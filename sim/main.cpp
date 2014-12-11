//
//  main.cpp
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
// you can build this file using:
// clang++ -stdlib=libc++ -std=gnu++11 -g -O0 main.cpp

#include "main.h"



//"ported" arduino definitions
#include "arduino_port/Arduino.h"
#include "arduino_port/Serial.h"
#include "arduino_port/EEPROM.h"

//different implementation for native client
#include "randomseed.h"

//extended eeprom
#include "../EEPROMAnything.h"
#include "../Neuron.h"
#include "../NeuralNetwork.h"

#define DEBUG

//arbot
#include "Debug.h"
#include "../Population.h"



int main()
{
	NeuralNetwork net;
	
	float weights[6]={/*l2n1*/0.525157,/*l2n2*/0.804601,/*l3n1*/0.183411,0.014265,/*l3n2*/0.387660,0.371998};
	net.setWeights(weights);
	
	float inputs[]={(float)2.602};
	float outputs[2];
	net.setInputs(inputs,1);
	net.calculate();
	net.getOutputs(&outputs[0],2);
	return 0;
}

/*
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

*/