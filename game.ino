#include "Player.cpp"
const int gameDuration = 30*1000;

const int nrOfMoles = 20;
const int nrOfPlayers = 2;
const unsigned long gameLength = 50000;
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

void setup() {
  randomSeed(analogRead(0));
  randomSeed(analogRead(0));
  randomSeed(analogRead(0));
  delay(11);
  randomSeed(analogRead(0));  
  Serial.begin(9600);

  
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
      p1->reset(firstButtonPin, firstp1Light, initialMoleLife);//TODO refactor
      p2->reset(firstButtonPin, firstp2Light, initialMoleLife);
      lightLights(p1->getMoles());
      lightLights(p2->getMoles());
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
    for(int i=0;i<3;i++){
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












