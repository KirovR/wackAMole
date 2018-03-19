#pragma once
#include "Arduino.h"

struct Mole {
  int buttonPin;
  int lightPin;
  int lastButtonState;
  bool moleActive;
  unsigned long moleLitTime;

  void activateMole(){
    moleActive = true;
    moleLitTime = millis();
    }

  void deactivateMole(){
    moleActive = false;
    moleLitTime = 0;
    }  

  boolean shouldKill(unsigned long currentTime, unsigned long life){
    if(moleActive && ((moleLitTime + life) < currentTime ) ){
       return true;
    }
    return false;  
  }  
} ;
