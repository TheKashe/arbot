//
//  ArbotController.h
//  
//
//  Created by Jernej Kaše on 11/20/14.
//
//

#ifndef ____ArbotController__
#define ____ArbotController__

#include "ArbotPlatform.h"

class ArbotController{
protected:
  ArbotPlatform *arbotPlatform;
public:
  ArbotController(ArbotPlatform *arbotPlatform){
    this->arbotPlatform=arbotPlatform;
  }

  //all inherited classes must call this, or things will fail to work
  virtual void setup(){
    arbotPlatform->setup();
  }
  
  virtual void loop() = 0;
};

#endif /* defined(____ArbotController__) */
