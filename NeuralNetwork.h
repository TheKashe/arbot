//
//  NeuralNetwork.h
//  arbot
//
//  Created by Jernej Kaše on 12/11/14.
//  Copyright (c) 2014 Kase. All rights reserved.
//
//  This is the simplest possible, static neural network implementation we need for arbot

#ifndef arbot_NeuralNetwork_h
#define arbot_NeuralNetwork_h

#include "Neuron.h"

class NeuralNetwork{
private:
	Neuron inputsLayer[1];
	Neuron hiddenLayer[2];
	Neuron outputsLayer[2];
	Neuron* pNeurons[3];
public:
	NeuralNetwork(){
		outputsLayer[0].init(2,0);
		outputsLayer[1].init(2,0);
		 
		hiddenLayer[0].init(1,2);
		hiddenLayer[1].init(1,2);
		hiddenLayer[0].setReceiver(0,&outputsLayer[0]);
		hiddenLayer[0].setReceiver(1,&outputsLayer[1]);
		hiddenLayer[1].setReceiver(0,&outputsLayer[0]);
		hiddenLayer[1].setReceiver(1,&outputsLayer[1]);
		
		inputsLayer[0].init(1,2);
		inputsLayer[0].setReceiver(0,&hiddenLayer[0]);
		inputsLayer[0].setReceiver(1,&hiddenLayer[1]);
		
		pNeurons[0]=&inputsLayer[0];
		pNeurons[1]=&hiddenLayer[0];
		pNeurons[2]=&outputsLayer[0];

		reset(); //just to be sure
	}
	
	byte getLevelCount()
	{
		return 3;
	}
	
	byte getNeuronCount(byte level)
	{
		switch(level){
			case 0: return 1;
			case 1: return 2;
			case 2: return 2;
		}
		return 0;
	}
	
	
	//weights are flat array since that's easy to use with our genetic framework
	void setWeights(float weights[]){
		int w=0; //current weight counter
		for(byte i=0;i<getLevelCount();i++){
			//here we are in a level
			for(byte j=0;j<getNeuronCount(i);j++){
				//here we are in a neuron
				for(byte k=0;k<pNeurons[i][j].getWeightCount();k++){
					//here we are at weight
					pNeurons[i][j].setNextWeight(i==0?1:weights[w++]); //input neurons habe no weights
				}
			}
			
		}
	}
	
	
	//this will reset the whole network
	void reset()
	{
		for(int i=0;i<(sizeof inputsLayer/sizeof inputsLayer[0]);i++){
			inputsLayer[i].reset();
		}
	}
	
	void setInputs(float inputs[], byte size)
	{
		for(int i=0;i<size;i++){
			inputsLayer[i].setNextInput(inputs[i]);
		}
		
	}
	
	void calculate(){
		inputsLayer[0].calculateAndPropagate();
		hiddenLayer[0].calculateAndPropagate();
		hiddenLayer[1].calculateAndPropagate();
		outputsLayer[0].calculate();
		outputsLayer[1].calculate();
	}
	
	void getOutputs(float *outputs, byte size){
		if(size!=2) return;
		for(int i=0;i<2;i++){
			outputs[i]=outputsLayer[i].getOutput();
		}
	}
	
};

#endif
