//
//  nn.h
//  arbot
//
//  Created by Jernej Kaše on 12/13/14.
//  Copyright (c) 2014 Kase. All rights reserved.
//
// to build with fann:
// clang++ -stdlib=libc++ -std=gnu++11 -I /Volumes/DataHD/MacOs/jernej/Developer/FANN-2.2.0-Source/src/include -g -O0 nn.cpp -L/Volumes/DataHD/MacOs/jernej/Developer/FANN-2.2.0-Source/bin -lfloatfann

#ifndef __arbot__nn__
#define __arbot__nn__

#include <iostream>
#include "../arduino_port/Arduino.h"
#include <fann.h>
#include "../../NeuralNetwork.h"


#define ARC4RANDOM_MAX      0x100000000
#define RND ((float)arc4random() / ARC4RANDOM_MAX)



void plotResults(double* xData, double* yData, int dataSize) {
	FILE *gnuplotPipe,*tempDataFile;
	char *tempDataFileName;
	double x,y;
	int i;
	tempDataFileName = "tempData";
	gnuplotPipe = popen("gnuplot","w");
	if (gnuplotPipe) {
		//fprintf(gnuplotPipe,"plot \"%s\" u 1:2 w l, '' u 1:3 w l\n",tempDataFileName);
		
		fprintf(gnuplotPipe,"plot \"%s\" with lines\n",tempDataFileName);
		fflush(gnuplotPipe);
		tempDataFile = fopen(tempDataFileName,"w");
		for (i=0; i <= dataSize; i++) {
			fprintf(tempDataFile,"%lf %lf\n",xData[i],yData[i]);
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

int main(){
	//init fann
	struct fann *ann = fann_create_standard(3, 1, 2, 1);
	fann_type *output;
	fann_type input[2];
	input[0]=-1.0;
	input[1]=0;
	fann_randomize_weights(ann, -1,1);
	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
    fann_set_activation_function_output(ann, FANN_SIGMOID);
	fann_connection cn[10000];
	fann_get_connection_array(ann,&cn[0]);
	
	//transform fann no NN??
	float weights[10000];
	for(byte i=0;i<7;i++){
		weights[i]=cn[i].weight;
	}
	
	NeuralNetwork nn(3,1,2,1);
	//float weights[]={-0.6,-0.2,-0.1,0.1,0.2,0.6,0.7};
	
	
	nn.setWeights(&weights[0]);
	float inputs[1];
	float outputs[1];
	
	double xData[35000];
	double yData[35000];
	uint32_t k=0;
		
	for(float i=-1;i<=1;i+=0.01){
		inputs[0]=i;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=i;
		yData[k]=outputs[0];
		k++;
	}
	
	plotResults(&xData[0],&yData[0],k-1);
	
	/*inputs[0]=0.123;
	for(uint32_t i=0;i<=30000000;i++){
		nn.calculate(&inputs[0],&outputs[0]);
	}*/
	
	/*input[0]=0.123;
	for(uint32_t i=0;i<=30000000;i++){
		output = fann_run(ann, input);
	}*/
	
	double xData1[35000];
	double yData1[35000];
	uint32_t k1=0;
	
	for(float i=-1;i<=1;i+=0.01){
		input[0]=i;
		output = fann_run(ann, input);
		xData1[k1]=i;
		yData1[k1]=output[0];
		k1++;
	}
	
	plotResults(&xData1[0],&yData1[0],k1-1);
	//fann_save(ann,"ann.net");
	fann_destroy(ann);
	
	return 0;
}

#endif /* defined(__arbot__nn__) */
