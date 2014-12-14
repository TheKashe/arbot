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



#define ARC4RANDOM_MAX      0x100000000



void plotResults(double* xData, double* yData, double* y1Data, int dataSize) {
	FILE *gnuplotPipe,*tempDataFile;
	char *tempDataFileName;
	double x,y;
	int i;
	tempDataFileName = "tempData";
	gnuplotPipe = popen("gnuplot","w");
	if (gnuplotPipe) {
		fprintf(gnuplotPipe,"plot \"%s\" u 1:2 w l, '' u 1:3 w l\n",tempDataFileName);
		
		//fprintf(gnuplotPipe,"plot \"%s\" with lines\n",tempDataFileName);
		fflush(gnuplotPipe);
		tempDataFile = fopen(tempDataFileName,"w");
		for (i=0; i <= dataSize; i++) {
			fprintf(tempDataFile,"%lf %lf %lf\n",xData[i],yData[i],y1Data[i]);
		}
		fclose(tempDataFile);
		printf("press enter to continue...");
		getchar();
		remove(tempDataFileName);
		fprintf(gnuplotPipe,"exit \n");
	} else {
		printf("gnuplot not found...");
	}
}

float stepNeuron(float x, float start, float end, float height){
	if(x>=start && x <=end) return height;
	return 0;
}

int main(){
	/*arduinoRandomize();
	testSigmoid();
	return 0;*/
	
	NeuralNetwork net;
	float weights[]={1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9};
	float inputs[]={1.1};
	float outputs[2];
	net.setWeights(weights);
	for(long i=0;i<30000000;i++){
	net.reset();
	net.setInputs(inputs,1);
	net.calculate();
	net.getOutputs(&outputs[0],2);
	}
}

/*Population< Genome<float> >::seedPopulation();
 Population< Genome<float> >::listPopulation();
 Population< Genome<float> >::evolve();
 Population< Genome<float> >::listPopulation();
 float weights[6];
 Genome<float> g(10);
 g.getAllGenes(weights);
 NeuralNetwork net;*/
/*
 float inputs[]={sensorNor};
 
 float outputs[2];
 net.setWeights(inputs);
 net.reset();
 net.setInputs(inputs,1);
 net.calculate();
 net.getOutputs(&outputs[0],2);*/
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