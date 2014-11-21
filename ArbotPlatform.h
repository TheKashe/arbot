//
//  ArbotPlatform.h
//  
//
//  Created by Jernej Kaše on 11/20/14.
//
//

#include "Arduino.h"

#ifndef ____ArbotPlatform__
#define ____ArbotPlatform__

class ArbotPlatform{
protected: 
  unsigned char normalizeSpeed(unsigned char speed){
    if(speed>100) speed=100;
    return speed;
  }
public:
  virtual void setup() =0;
  virtual void forwards(unsigned char speed) =0;
  virtual void backwards(unsigned char speed) =0;
  virtual void left(unsigned char speed) =0;
  virtual void right(unsigned char speed) =0;
  virtual void fullStop() =0;
};

#endif /* defined(____ArbotPlatform__) */
