#include "Debug.h"
#include "Arduino.h"
#include "randomseed.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include "ArbotVMA03MDPlatform.h"
#include "FreedomController.h"
#include "Neuron.h"
#include "NeuralNetwork.h"



//definitions for controller
#define IR_PIN         11      //where is the IR sensor connected
#define NO_COMMAND_TIMEOUT 250 //if we don't receive IR signal in this many cycles, 

//definitions for VMA03 controller, change those to match your pins!
#define PWM_A       3
#define PWM_B       9
#define DIR_A       4
#define DIR_B       8
#define BLINK_PIN   13


void setup(){
  Serial.begin(9600);
}

void loop() {
  Serial.println("Starting");
  NeuralNetwork net;
	
	float weights[6]={/*l2n1*/0.525157,/*l2n2*/0.804601,/*l3n1*/0.183411,0.014265,/*l3n2*/0.387660,0.371998};
	net.setWeights(weights);
	
	float inputs[]={(float)2.602};
	float outputs[2];
	for(long i=0;i<100000;i++){
		net.reset();
		net.setInputs(inputs,1);
		net.calculate();
		net.getOutputs(&outputs[0],2);
	}
    Serial.print("done:");
    Serial.print(outputs[0]);
    Serial.print(",");
    Serial.println(outputs[0]);

}
