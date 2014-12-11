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
#ifndef ____IArbotVMA03MDPlatform__
#define ____IArbotVMA03MDPlatform__

#include "ArbotPlatform.h"

#define PING_NO_ANSWER	65534
#define PING_WAITING	65533
#define PING_TIMEOUT	5000

class IArbotVMA03MDPlatform: virtual public ArbotPlatform{
public:
    virtual boolean isMoving()=0;
	//sequence initialization
	virtual void init()=0;
	//sequence ended
    virtual void terminate()=0;
    virtual boolean isFree()=0;
    virtual void showDiagnostics(const char*)=0;
	virtual void ping()=0;
	virtual void pingback()=0;			//this is probably only useful for simulator - we need to get info when the ping is back
	virtual uint16_t getPingReply()=0;
	
	
	
	
};

#endif /* defined(____ArbotVMA03MDPlatform__) */
