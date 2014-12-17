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



FILE *gnuplotPipe;

void plotResults(double* xData, double* yData, int dataSize) {
	FILE *tempDataFile;
	char *tempDataFileName;
	double x,y;
	int i;
	tempDataFileName = "tempData";
	//gnuplotPipe = popen("gnuplot","w");
	if (gnuplotPipe) {
		//fprintf(gnuplotPipe,"clear\n");
		//fprintf(gnuplotPipe,"plot \"%s\" u 1:2 w l, '' u 1:3 w l\n",tempDataFileName);
		fprintf(gnuplotPipe,"plot \"%s\" with lines\n",tempDataFileName);
		fflush(gnuplotPipe);
		tempDataFile = fopen(tempDataFileName,"w");
		for (i=0; i <= dataSize; i++) {
			fprintf(tempDataFile,"%lf %lf\n",xData[i],yData[i]);
		}
		fclose(tempDataFile);
		fflush(gnuplotPipe);
		//printf("press enter to continue...");
		//getchar();
		//remove(tempDataFileName);
		//fprintf(gnuplotPipe,"exit \n");
	} else {
		printf("gnuplot not found...");
	}
}

void plotTraining(){
	//printf("press enter to continue...");
	//getchar();
	NeuralNetwork nn(3,1,200,1);
	float inputs[]={0.3,0,-0.7};
	float outputs[1];
	float targets[]={0.572,0.1,0.897};
	float err;
	//float weights[]={0.1,0.2,0.3,0.4,0.5,0.6,0.7};
	//nn.setWeights(&weights[0]);
	nn.randomize();
	int i=0;
	double xData[35000];
	double yData[35000];
	
	gnuplotPipe= popen("gnuplot","w");
	//fprintf(gnuplotPipe,"set multiplot\n");
	fprintf(gnuplotPipe,"set yrange [0:1]\n");
	fprintf(gnuplotPipe,"set xrange [-1:1]\n");
	
	nn.calculate(&inputs[0],&outputs[0]);
	do{
		nn.backprop(&targets[0],0.7);
		nn.calculate(&inputs[1],&outputs[0]);
		nn.backprop(&targets[1],0.7);
		nn.calculate(&inputs[2],&outputs[0]);
		nn.backprop(&targets[2],0.7);
		//plot net before training
		nn.calculate(&inputs[0],&outputs[0]);
		err=targets[0]-outputs[0];
		err=err<0?-err:err;
		std::cout << outputs[0] << ":"<< err << "\n";
		i++;
		uint32_t k=0;
		for(float p=-1;p<=1;p+=0.01){
			inputs[0]=p;
			nn.calculate(&inputs[0],&outputs[0]);
			xData[k]=p;
			yData[k]=outputs[0];
			k++;
		}
		plotResults(&xData[0],&yData[0],k-1);
	}while(1);//(err>0.01 && i<10000);
	uint32_t k=0;
	for(float p=-1;p<=1;p+=0.01){
		inputs[0]=p;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=p;
		yData[k]=outputs[0];
		k++;
	}
	plotResults(&xData[0],&yData[0],k-1);
	fprintf(gnuplotPipe,"exit \n");
}


void simpleBackprop()
{
	//printf("press enter to continue...");
	//getchar();
	NeuralNetwork nn(3,1,2,1);
	float inputs[]={0.3};
	float outputs[1];
	float targets[]={0.572};
	float err;
	float weights[]={0.1,0.2,0.3,0.4,0.5,0.6,0.7};
	nn.setWeights(&weights[0]);
	
	int i=0;
	do{
		nn.calculate(&inputs[0],&outputs[0]);
		nn.backprop(&targets[0], 0.7);
		err=targets[0]-outputs[0];
		err=err<0?-err:err;
		std::cout << outputs[0] << ":"<< err << "\n";
		i++;
	}while(err>0.01 && i<10000);
	
}

int main(){
	
	plotTraining();
	//simpleBackprop();
	return 0;
}

#endif /* defined(__arbot__nn__) */
