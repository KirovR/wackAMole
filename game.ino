#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "Player.cpp"
const int gameDuration = 30*1000;

const int nrOfMoles = 20;
const int nrOfPlayers = 2;
const unsigned long gameLength = 35000;
const unsigned long initialMoleLife = 2000;
const unsigned long afterGameBuffer = 4000;

Mole moles[20];
Player* p1 = NULL;
Player* p2 = NULL;

boolean gameInProgress;
unsigned long gameStartTime;
unsigned long gameEndTime;
unsigned long currentTime;

const int firstButtonPin = 22;
const int firstp1Light = 2;
const int firstp2Light = 42;
const int buzzer = 12;

int warmUpAnimationFrames[3][6]={
    {0,1,2,10,11,12},
    {4,5,14,15,-1,-1},
    {8,18,-1,-1,-1,-1} 
   };

unsigned long idleAnimationSpeed = 1000;
unsigned long idleAnimationLastFrame=0;
int idleAnimationFrame=0;
//int idleAnimationFrames[2][10]={
//    {0,2,4,5,8,10,12,14,15,18},
//    {1,4,5,7,9,11,14,15,17,19}
//};

int idleAnimationFrames[2][12]={
    {0,2,3,6,7,9,10,12,13,16,17,19},
    {1,4,5,8,11,14,15,18,-1,-1,-1,-1}
};    

int buttonState = 0;

Adafruit_7segment p1ScoreDisplay = Adafruit_7segment();
Adafruit_7segment p2ScoreDisplay = Adafruit_7segment();

int p1LastScore = -1;
int p2LastScore = -1;

void setup() {
  randomSeed(analogRead(0));
  randomSeed(analogRead(0));
  randomSeed(analogRead(0));
  delay(11);
  randomSeed(analogRead(0));  
  Serial.begin(9600);

  p1ScoreDisplay.begin(0x70);
  p2ScoreDisplay.begin(0x71);
  
  pinMode(buzzer,OUTPUT);
  for(int i=0; i<nrOfMoles;i++){ //buttons
   pinMode(firstButtonPin + i, INPUT_PULLUP);
  }
  for(int i=0; i<10;i++){ //p1 lights
    pinMode(firstp1Light + i, OUTPUT);
  }
  for(int i=0; i<10;i++){ //p1 lights
    pinMode(firstp2Light + i, OUTPUT);
  }
 
   p1 = new Player(1, firstButtonPin, firstp1Light, initialMoleLife,  &Serial);
   p2 = new Player(2, firstButtonPin+10, firstp2Light, initialMoleLife, &Serial);

 
//   printScore(p2->getPlayerNumber(), 10000); //startup game ----
//   delay(1000);

  p1ScoreDisplay.print(0xBEEF, HEX);
  p1ScoreDisplay.writeDisplay();
//  delay(1000);


  p2ScoreDisplay.print(0xBEEF, HEX);
  p2ScoreDisplay.writeDisplay();
  delay(1000);


//   printScore(p2->getPlayerNumber(), 1338); //startup game ----
//   delay(1000);

   printScore(p1->getPlayerNumber(), 0); //startup game ----
   printScore(p2->getPlayerNumber(), 0); //startup game ----
   delay(1000);

   


   gameEndTime = 0;
   gameInProgress = false;
   
}
void loop() {
  currentTime = millis() + gameLength;
  decideGameState();
  if(gameInProgress){
    handlePlayerActions(p1);
    handlePlayerActions(p2);
  }
  
}

 void handlePlayerActions(Player  *player){
  boolean change;
   for(int i=0;i<10;i++){
     buttonState  = digitalRead(firstButtonPin+ i + (player->getPlayerNumber() - 1)*10);
     boolean somethingChanged = player->handleButtonPress(i,buttonState);
     change = change || somethingChanged;
   }
   boolean moleChanged =  player->handleDeadMoles();
   change = change || moleChanged;

   if(change){
    lightLights(player->getMoles());
   }

   printScore(player->getPlayerNumber(), player->getScore());
 }
void startGame(){
    warmUpAnimation();
    gameInProgress = true; 
    gameStartTime = currentTime;
    p1->lightInitial();
    p2->lightInitial();
    
}

void decideGameState(){
  if(!gameInProgress){
    if(currentTime - afterGameBuffer > gameEndTime){
    idleAnimation();
    handleRandomButtonPress();
    }
  }else{
    if(gameStartTime + gameLength < currentTime ){//game ended
      gameInProgress = false;
      gameEndTime = currentTime;

      int p1FinalScore = p1->getScore();
      int p2FinalScore = p2->getScore();

      p1->reset(firstButtonPin, firstp1Light, initialMoleLife);//TODO refactor
      p2->reset(firstButtonPin, firstp2Light, initialMoleLife);
      lightLights(p1->getMoles());
      lightLights(p2->getMoles());

      delay(1500);
      if (p1FinalScore > p2FinalScore) {
          buzz(0);
      } else if (p2FinalScore > p1FinalScore) {
        buzz(0);
        delay(400);
        buzz(0);
      } else {
        buzz(20);
        delay(400);
        buzz(20);
        delay(400);
        buzz(20);
      }
    }
  }
}

void handleRandomButtonPress(){
  for(int i=0; i<nrOfMoles;i++){
    buttonState  = digitalRead(firstButtonPin+i);
    if(!buttonState){
      startGame();
      break;
    }   
  }
}

void printScore(int player, int number){

    Adafruit_7segment targetDisplay;
    if(player == 1){
      if(p1LastScore == number){
        return;  
      }
      p1LastScore = number;
      targetDisplay = p1ScoreDisplay;
    }else{
      if(p2LastScore == number){
        return;  
      }
      p2LastScore = number;
      targetDisplay = p2ScoreDisplay;
    }
    targetDisplay.print(number);
    targetDisplay.writeDisplay();
}

void lightLights(Mole *litLeds){
  for(int i=0; i<10; i++){
      digitalWrite(litLeds[i].lightPin, litLeds[i].moleActive );
  }
}

void idleAnimation(){
    if(idleAnimationLastFrame + idleAnimationSpeed < currentTime ){
        for(int k=0;k<20;k++){
          digitalWrite(k<10 ? k+firstp1Light : k-10+firstp2Light, LOW);
        }
        for(int j=0;j<12;j++){
           if(idleAnimationFrames[idleAnimationFrame][j] == -1){
              continue;
           }
             digitalWrite(idleAnimationFrames[idleAnimationFrame][j]<10 ? idleAnimationFrames[idleAnimationFrame][j]+firstp1Light : (idleAnimationFrames[idleAnimationFrame][j]-10+firstp2Light), HIGH);
         }
       idleAnimationFrame = !idleAnimationFrame;
       idleAnimationLastFrame = currentTime;
    }
  
}

void warmUpAnimation(){
  int additionalFreq = 0;
    for(int i=0;i<3;i++){// three frames
      printScore(1, (3-i));
      printScore(2, (3-i));
       for(int k=0;k<20;k++){
          digitalWrite(k<10 ? k+firstp1Light : k-10+firstp2Light, LOW);
       }
       for(int j=0;j<6;j++){
        if(warmUpAnimationFrames[i][j] == -1){
          continue;
        }
           digitalWrite(warmUpAnimationFrames[i][j]<10 ? warmUpAnimationFrames[i][j]+firstp1Light : (warmUpAnimationFrames[i][j]-10+firstp2Light), HIGH);
       }
       buzz(additionalFreq);
       additionalFreq+=15; 
       delay(800-220*i);
        
    }
    for(int k=0;k<20;k++){
          digitalWrite(k<10 ? k+firstp1Light : k-10+firstp2Light, LOW);
    }

}
void buzz(int additionalFreq){
  for(int i=0;i<80+additionalFreq;i++)
      {
        digitalWrite(buzzer,HIGH);
        delay(2);
       digitalWrite(buzzer,LOW);
        delay(1);
      }
  
  
}












