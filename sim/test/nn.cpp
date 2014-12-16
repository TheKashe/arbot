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

#include <time.h>

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
	int layers[]={1,2,1};
	struct fann *ann = fann_create_standard(3, layers[0],layers[1],layers[2]);
	fann_type *output;
	fann_type input[2];
	fann_type target[1];
	input[0]=0.3;
	input[1]=0;
	fann_randomize_weights(ann, -1,1);
	fann_set_activation_function_hidden(ann, FANN_SIGMOID);
    fann_set_activation_function_output(ann, FANN_SIGMOID);
	fann_connection cn[10000];
	fann_get_connection_array(ann,&cn[0]);
	
	cn[0].weight=0.1;
	cn[1].weight=0.2;
	cn[2].weight=0.3;
	cn[3].weight=0.4;
	cn[4].weight=0.5;
	cn[5].weight=0.6;
	cn[6].weight=0.7;
	//cn[7].weight=0.8;
	//cn[8].weight=0.9;
	//cn[9].weight=0.1;
	fann_set_weight_array(ann,&cn[0],7);
	
	int i=0;
	float targets[]={0.572};
	float err;
	output = fann_run(ann, input);
	do{
		fann_train(ann,input,targets);
		output = fann_run(ann, input);
		err=targets[0]-output[0];
		err=err<0?-err:err;
		std::cout << output[0] << ":"<< err << "\n";
		i++;
	}while(err>0.01 && i<10000);
	
	//plot fann
	/*double xData1[35000];
	double yData1[35000];
	uint32_t k1=0;
	
	for(float i=-1;i<=1;i+=0.01){
		input[0]=i;
		output = fann_run(ann, input);
		xData1[k1]=i;
		yData1[k1]=output[0];
		k1++;
	}
	
	plotResults(&xData1[0],&yData1[0],k1-1);*/
	
	//transform fann no NN
	float weights[10000];
	uint16_t weightsCount=(layers[0]+1)*layers[1]+(layers[1]+1)*layers[2];
	for(uint16_t i=0;i<weightsCount;i++){
		weights[i]=cn[i].weight;
	}
	
	NeuralNetwork nn(3,layers[0],layers[1],layers[2]);
	
	nn.setWeights(&weights[0]);
	float inputs[1];
	float outputs[1];
	inputs[0]=0.3;
	nn.calculate(&inputs[0],&outputs[0]);
	
	//plot nn
	/*double xData[35000];
	double yData[35000];
	uint32_t k=0;
	
	for(float i=-1;i<=1;i+=0.01){
		inputs[0]=i;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=i;
		yData[k]=outputs[0];
		k++;
	}
	plotResults(&xData[0],&yData[0],k-1);*/

	
	/*clock_t t = clock();

	float r=0;
	inputs[0]=0.123;
	for(uint32_t i=0;i<=30000000;i++){
		nn.calculate(&inputs[0],&outputs[0]);
		r+=outputs[0];
	}
	t = clock() - t;
	printf ("It took me %lu clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
	std::cout << r;
	std::cout<<outputs[0];*/
		
	
	
	/*input[0]=0.123;
	for(uint32_t i=0;i<=30000000;i++){
		output = fann_run(ann, input);
	}*/
	
	fann_destroy(ann);
	return 0;
}

#endif /* defined(__arbot__nn__) */
