//
//  main.cpp
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#include "main.h"

//"ported" arduino definitions
#include "arduino_port/Arduino.h"
#include "arduino_port/Serial.h"
#include "arduino_port/EEPROM.h"

//different implementation for native client
#include "randomseed.h"

//extended eeprom
#include "../EEPROMAnything.h"

#define DEBUG

//arbot
#include "Debug.h"
#include "../Population.h"



int main()
{
	Population::seedPopulation();
	Population::listPopulation();
	int i=0;
	while(true){
		Population::evolve();
		Population::listPopulation();
		DEBUG_STDOUT("\n\n");
		DEBUG_STDOUT(i);
		DEBUG_STDOUT("\n\n");
		i++;
	}
	return 0;
}