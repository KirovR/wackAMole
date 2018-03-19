#include <LinkedList.h>
#include "Mole.cpp"
#include <HardwareSerial.h>
#include "Arduino.h"
class Player{
  private:
    const int maximumActiveMoles = 5; 
    const int minimumActiveMoles = 2;
    int playerNr;
    unsigned int score;
    Mole moles[10];
    int nrOfDeactivatedMoles;
    HardwareSerial * customSerial;
    unsigned long initialMoleLife;
    boolean lightsModified = false;
    unsigned long lastDeadMolesCheck;
    
  public:
    Player(int playerNr, int startingButtonPosition, int startingLightPosition, unsigned long initialMoleLife, HardwareSerial *serial){
        this->playerNr = playerNr;
        customSerial = serial;
        reset(startingButtonPosition, startingLightPosition, initialMoleLife);
    }

    void lightInitial(){
       for(int i=0; i<3; i++){
          activateRandomMole();
        }
        lastDeadMolesCheck = millis();
   }

    boolean handleButtonPress(int index, int buttonState){
          lightsModified = false;
          if (buttonState != moles[index].lastButtonState){
            if(buttonState == 0){//button pressed
               if(moles[index].moleActive){
                   score+=10;
                   int molesToActivate;
                   if((10 - nrOfDeactivatedMoles - 1 ) <= minimumActiveMoles ){
                    molesToActivate = random(1, maximumActiveMoles - (10 - nrOfDeactivatedMoles ) + 2);
                    }else{
                      molesToActivate = random(maximumActiveMoles - (10 - nrOfDeactivatedMoles ) + 2);
                    }
                   for(int i=0; i<molesToActivate; i++){
                    activateRandomMole();
                   }
                   moles[index].deactivateMole();
                   ++nrOfDeactivatedMoles;
                   lightsModified = true;
               }else{
                  if(score>0){
                     score-=5;
                  }
               }
            }
          moles[index].lastButtonState = buttonState;
          delay(6); //to rpevent bouncing
       }
       return lightsModified;
    }

    boolean handleDeadMoles(){
      lightsModified = false;
      unsigned long currentTime = millis();
      if(lastDeadMolesCheck + 1000UL < currentTime){ //only perform the check once a second
        return lightsModified;
      }
       
      lastDeadMolesCheck = currentTime;
      for(int i=0; i< 10; i++){   
        if(moles[i].shouldKill(currentTime,initialMoleLife)){
           --score;
           activateRandomMole();
            moles[i].deactivateMole();
           ++nrOfDeactivatedMoles;
           lightsModified = true;
        }
      }
      return lightsModified;
    }

    void activateRandomMole(){
      int moleToActivate = random(nrOfDeactivatedMoles);
      int reachedIndex = -1;
      for(int i=0; i<10; i++){
        if(!moles[i].moleActive){
            if(++reachedIndex==moleToActivate){
              moles[i].activateMole();
              --nrOfDeactivatedMoles;
             
              
              break;
            }
        }
      }
    }

    Mole getMole(int index){
      return moles[index];
    }

    void reset(int startingButtonPosition, int startingLightPosition, unsigned long initialMoleLife){
        score = 0;
        nrOfDeactivatedMoles = 9;
        this->initialMoleLife = initialMoleLife;
        for(int i=0; i<10;i++){
           moles[i] = {(startingButtonPosition + i), (startingLightPosition + i), 0, false, 0};
        }
        lastDeadMolesCheck = millis();
    }

    Mole *getMoles(){
      return moles;
    }

    int getPlayerNumber(){
      return  playerNr;
    }
  

};
