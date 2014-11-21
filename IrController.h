//
//  IrController.h
//  
//
//  Created by Jernej Kaše on 11/20/14.
//
//

#include "ArbotPlatform.h"
#include <IRremote.h>

#ifndef ____IrController__
#define ____IrController__

class IrController{
  ArbotPlatform *arbotPlatform;
  IRrecv *irrecv;
  unsigned long lastCommandTimestamp; //when did we receive last IR command
  unsigned int noCommandTimout;
public:
  IrController(ArbotPlatform *arbotPlatform, char recvPin=11, unsigned int noCommandTimout=100){
    this->arbotPlatform=arbotPlatform;
    this->irrecv = new IRrecv(recvPin);
    lastCommandTimestamp=millis();
    this->noCommandTimout=noCommandTimout;
  }

  virtual void setup(){
    irrecv->enableIRIn(); // Start the receiver
    arbotPlatform->setup();
  }
  
  virtual void loop(){
    decode_results results;
    if (irrecv->decode(&results)){
      if(results.decode_type==RC5){
        switch(results.value)
        {
          case 0x820: arbotPlatform->forwards(100); lastCommandTimestamp=millis(); break;
          case 0x20:  arbotPlatform->forwards(100); lastCommandTimestamp=millis(); break;
          case 0x810: arbotPlatform->left(100); lastCommandTimestamp=millis(); break;
          case 0x10:  arbotPlatform->left(100); lastCommandTimestamp=millis(); break;
          case 0x821: arbotPlatform->backwards(100); lastCommandTimestamp=millis(); break;
          case 0x21:  arbotPlatform->backwards(100); lastCommandTimestamp=millis(); break;
          case 0x811: arbotPlatform->right(100); lastCommandTimestamp=millis(); break;
          case 0x11:  arbotPlatform->right(100); lastCommandTimestamp=millis(); break;
          default:    arbotPlatform->fullStop(); lastCommandTimestamp=millis();
        }
      }
      //Serial.println(results.value, HEX);
      //dump(&results);
      irrecv->resume(); // Receive the next value
    } 
    else
    {
      unsigned int noCommand=millis()-lastCommandTimestamp;
      //Serial.println(no_command);
      if(noCommand>=noCommandTimout) {
        arbotPlatform->fullStop();
        lastCommandTimestamp=millis();
      }
    }
  }
};

#endif /* defined(____IrController__) */
