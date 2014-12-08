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
#include "IArbotVMA03MDPlatform.h"

#define STALL_DETECTION_MAGIC_NUMBER 4  //4 seems to be good number higher number is more sensitive!

class ArbotVMA03MDPlatform: public ArbotVMA03Platform, public IArbotVMA03MDPlatform{
private:
	// this is for the maze solving robot
	// when sesor detects change, interrupt will set it to one
	// this means the robot has accomplished the mission
	// problem: due to interrupt the method is static
	// and so is this variable, which prevents multiple robots in a sim
	static boolean _isFree;
	//as used in static method, this is static, too :-/
	static byte _freedomInterruptNo; // must match sensor pin, but depends on arduino model
	
	byte motionSensorPin;
	int  motionSensorStill;  // analog read while the platform is still
	byte freedomSensorPin;   // the spikes "sensor"
	
public:
	ArbotVMA03MDPlatform(unsigned char pwm_a=3,unsigned char pwm_b=9,
						 unsigned char dir_a=4,unsigned char dir_b=8,
						 byte blink_pin=13, byte motionSensorPin=0, byte freedomInterrNo=4):ArbotVMA03Platform(pwm_a,pwm_b,dir_a,dir_b,blink_pin){
		this->motionSensorPin  = motionSensorPin;
		this->freedomSensorPin = freedomSensorPin;
		_freedomInterruptNo = freedomInterrNo;
	}
	
	virtual void setup(){
		_isFree=false; //when robot starts it's not free
		ArbotVMA03Platform::setup();
		motionSensorStill = analogRead(motionSensorPin);
		pinMode(freedomSensorPin,INPUT_PULLUP);
		attachInterrupt(_freedomInterruptNo,freedomInterrupt,LOW);
	}
	
	virtual void init(){
		_isFree=false; //when robot starts it's not free
		attachInterrupt(_freedomInterruptNo,freedomInterrupt,LOW);
	};
	
	virtual void terminate(){
		detachInterrupt(_freedomInterruptNo);
	}
	
	virtual boolean isMoving(){
		int motionSensorReadout = analogRead(motionSensorPin);    // read the input pin
		if(abs(motionSensorReadout-motionSensorStill)<=STALL_DETECTION_MAGIC_NUMBER){
			return false;
		}
		return true;
	}
	
	static void freedomInterrupt()
	{
		Serial.println("freedom interrupt!");
		_isFree=true;
		detachInterrupt(_freedomInterruptNo); //we reset before next run anyway
	}
	
	virtual void showDiagnostics(const char* diag){
		Serial.println(diag);
	}
	
	virtual boolean isFree(){
		return _isFree;
	}
	
};

//definitions of static members
byte ArbotVMA03MDPlatform::_freedomInterruptNo = 0;
boolean ArbotVMA03MDPlatform::_isFree = false;

#endif /* defined(____ArbotVMA03MDPlatform__) */
