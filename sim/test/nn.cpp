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
	struct fann *ann = fann_create_standard(3, 2, 200, 1);
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
	float biases[10000];
	int layers[]={2,200,1}; //must be the same as fann
	byte  layerCount=3;
	int weightsCount;
	int	  cnIdx=0;
	int	  wIdx=0;
	int   bIdx=0;
	for(byte i=1;i<layerCount;i++){
		weightsCount=(layers[i-1]+1)*layers[i];
		for(int j=0;j<weightsCount;j++){
			if((j+1)%(layers[i-1]+1)==0){ //if j%(layers[i-1]+1)==0, we are dealing with bias
				biases[bIdx++]=cn[cnIdx++].weight;
			}
			else{
				weights[wIdx++]=cn[cnIdx++].weight;
			}
		}
	}
	
	
	NeuralNetwork nn(3,2,200,1);
	//float weights[]={-0.6,-0.2,-0.1,0.1,0.2,0.6};
	//float biases[]={-0.5,-0.3,0.3,0.5};
	
	nn.setWeights(&weights[0]);
	nn.setBiases(&biases[0]);
	float inputs[2];
	float outputs[1];
	
	double xData[35000];
	double yData[35000];
	uint32_t k=0;
	
	inputs[0]=0;
	nn.calculate(&inputs[0],&outputs[0]);
	
	inputs[0]=1;
	nn.calculate(&inputs[0],&outputs[0]);
	
	input[1]=0;
	for(float i=-1;i<=1;i+=0.01){
		inputs[0]=i;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=i;
		yData[k]=outputs[0];
		k++;
	}
	
	plotResults(&xData[0],&yData[0],k-1);
	
	
	
		
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
