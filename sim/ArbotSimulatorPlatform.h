//
//  ArbotSimulatorPlatform.h
//  
//
//  Created by Jernej Kaše on 11/22/14.
//
//
//
#ifndef ____ArbotSimulatorPlatform__
#define ____ArbotSimulatorPlatform__

#include "../IArbotVMA03MDPlatform.h"

#include <unistd.h>

class ArbotSimulatorPlatform: public ArbotPlatform, public IArbotVMA03MDPlatform{
private:
    boolean _isMoving;
    pp::Instance* _instance;
    boolean _isFree;
public:
  
  ArbotSimulatorPlatform(pp::Instance *instance){
    _isMoving=false;
    _isFree=false;
    _instance=instance;
  }

  virtual void setup(){
  }
	
	virtual void init(){
		_instance->PostMessage("INI");
	}
  
  virtual void terminate(){
    _instance->PostMessage("TRM");
    _isFree=false;
    _isMoving=false;
	sleep(1);
  }
      
  virtual void forwards(unsigned char speed){
    _instance->PostMessage("FWD");
    _isMoving=true;
  }

  virtual void backwards(unsigned char speed){
    _instance->PostMessage("BCK");
    _isMoving=true;
  }

  virtual void left(unsigned char speed){
    _instance->PostMessage("LFT");
    _isMoving=true;
  }
    
  virtual void right(unsigned char speed){
    _instance->PostMessage("RGH");
    _isMoving=true;
  }
    
  virtual void fullStop(){
    _instance->PostMessage("STOP");
    _isMoving=false;
  }

  virtual boolean isFree(){
    return _isFree;
  }

  virtual boolean isMoving(){
    return _isMoving;
  }

  void stalled(){
    _isMoving=false;
  }
  
  void freedomInterrupt()
  {
    _isFree=true;
  }
  
  virtual void showDiagnostics(const char* diag){
    _instance->PostMessage(diag);
  }

  virtual void blink(int time){
    
  }

};

#endif /* defined(____ArbotSimulatorPlatform__) */
