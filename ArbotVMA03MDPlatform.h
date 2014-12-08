//
//  ArbotVMA03MDPlatform.h
//  
//
//  Created by Jernej Kaše on 11/22/14.
//
//
//  VMA03 Motor Shield extended with motion detector sensor
//  For the sensor I use a small motor I found in an old CD player
//  This motor is mounted as voltage generator - it rotates with one of the tracks
//  It's connected to arduino over voltage divider, in my case all thhree resisotrs are 1.8k ohm
//  GND--| R1 |--| R2 | -- 3.3V (or 5V)
//              |
//              - | R3 | - | motor | - GND 
//
#ifndef ____ArbotVMA03MDPlatform__
#define ____ArbotVMA03MDPlatform__

#include "ArbotVMA03Platform.h"

#define STALL_DETECTION_MAGIC_NUMBER 4  //4 seems to be good number higher number is more sensitive!

class ArbotVMA03MDPlatform: public ArbotVMA03Platform{
  byte motionSensorPin;
  int motionSensorStill; // analog read while the platform is still
public:
    ArbotVMA03MDPlatform(unsigned char pwm_a=3,unsigned char pwm_b=9,unsigned char dir_a=4,unsigned char dir_b=8, byte motionSensorPin=0):ArbotVMA03Platform(pwm_a,pwm_b,dir_a,dir_b){
      this->motionSensorPin=motionSensorPin;
    }
    
    virtual void setup(){
      ArbotVMA03Platform::setup();
      motionSensorStill = analogRead(motionSensorPin);
    }
    
    boolean isMoving(){
      int motionSensorReadout = analogRead(motionSensorPin);    // read the input pin
      if(abs(motionSensorReadout-motionSensorStill)<=STALL_DETECTION_MAGIC_NUMBER){
        return false;
      }
      return true;
    }
};

#endif /* defined(____ArbotVMA03MDPlatform__) */
