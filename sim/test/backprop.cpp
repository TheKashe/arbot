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
	//printf("press enter to continue...");
	//getchar();
	NeuralNetwork nn(3,1,200,1);
	float inputs[]={0.3};
	float outputs[1];
	float targets[]={0.572};
	float err;
	//float weights[]={0.1,0.2,0.3,0.4,0.5,0.6,0.7};
	//nn.setWeights(&weights[0]);
	nn.randomize();
	int i=0;
	nn.calculate(&inputs[0],&outputs[0]);
	do{
		nn.backprop(&targets[0],0.7);
		nn.calculate(&inputs[0],&outputs[0]);
		err=targets[0]-outputs[0];
		err=err<0?-err:err;
		std::cout << outputs[0] << ":"<< err << "\n";
		i++;
	}while(err>0.01 && i<1000);

	/*double xData[35000];
	double yData[35000];
	//plot net before training
	uint32_t k=0;
	for(float i=-1;i<=1;i+=0.01){
		inputs[0]=i;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=i;
		yData[k]=outputs[0];
		k++;
	}
	plotResults(&xData[0],&yData[0],k-1);
	
	inputs[0]=0.3;
	float target=0.572;
	float err;
	
	int i=0;
	nn.calculate(&inputs[0],&outputs[0]);
	do{
		outputs[0]=target;
		nn.backprop(&outputs[0],1);
		nn.calculate(&inputs[0],&outputs[0]);
		err=outputs[0]-target;
		err=err<0?-err:err;
		std::cout << outputs[0] << ":"<< err << "\n";
		i++;
	}while(err>0.01 && i<100);
	

	
	//plot net after training
	k=0;
	for(float i=-1;i<=1;i+=0.01){
		inputs[0]=i;
		nn.calculate(&inputs[0],&outputs[0]);
		xData[k]=i;
		yData[k]=outputs[0];
		k++;
	}
	plotResults(&xData[0],&yData[0],k-1);*/
		return 0;
}

#endif /* defined(__arbot__nn__) */
