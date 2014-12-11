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
	uint32_t pingTimestamp;
	uint32_t pingbackTimestamp;
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
	
	virtual void ping()
	{
		DEBUG_STDOUT("pinging\n");
		_instance->PostMessage("PNG");
		pingTimestamp=millis();
		pingbackTimestamp=0;
	}
	
	virtual void pingback()
	{
		DEBUG_STDOUT("got pingback\n");
		pingbackTimestamp=millis();
	}
	
	virtual uint16_t getPingReply()
	{
		DEBUG_STDOUT("getPingReply:\n");
		if(pingbackTimestamp==0 && (millis()-pingTimestamp<PING_TIMEOUT)){
			DEBUG_STDOUT("PING_WAITING\n");
			return PING_WAITING;
		}
		if(pingbackTimestamp==0 && (millis()-pingTimestamp>PING_TIMEOUT)){
			DEBUG_STDOUT(millis()-pingTimestamp);
			DEBUG_STDOUT("PING_NO_ANSWER\n");
			return PING_NO_ANSWER;
		}
		uint16_t pingback=pingbackTimestamp-pingTimestamp;
		DEBUG_STDOUT("Got ping in:");
		DEBUG_STDOUT(pingback);
		return pingback;
	};
	
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
