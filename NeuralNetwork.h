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

#define ARC4RANDOM_MAX      0x100000000
#define RND ((float)arc4random() / ARC4RANDOM_MAX)

#define OUTPUTS (layers[layersCount-1])

class NeuralNetwork{
private:
	uint8_t layersCount;
	
	uint16_t* layersN;
	uint16_t* layers;
	float ***weightsVector;
	float **activationsVector;
	float **sumsVector;
	float *biases;
public:
	NeuralNetwork(byte layerCount, ...)
	{
		layersCount			= layerCount;
		
		layers	= new uint16_t[layerCount];
		layersN	= new uint16_t[layerCount];
		va_list param;
		va_start(param, layerCount);
		for (byte i = 0; i < layerCount; i++){
			layers[i]=va_arg (param, uint16_t);
			layersN[i]=layers[i];
			//let's add virtual bias neurons to first and hidden layers
			if(i<layerCount-1){
				layers[i]+=1;
			}
		}
		va_end (param);
	
		//transpose weights to multidemensional vector
		weightsVector			= new float**[layersCount]; //layer 0 actually never has weights, but let's simplify
		activationsVector		= new float*[layersCount];
		activationsVector[0]	= new float[layers[0]];		//activations also exist on this level
		sumsVector				= new float*[layersCount];
		
		activationsVector[0][layers[0]-1]=1;				//bias neuron for layer 0
		for(byte l=1;l<layersCount;l++){
			weightsVector[l]=new float*[layersN[l]];
			activationsVector[l]=new float[layers[l]];
			sumsVector[l]=new float[layersN[l]];
			for(uint16_t j=0;j<layersN[l];j++){
				weightsVector[l][j]=new float[layers[l-1]];
			}
			//all but last layer have bias neuron
			if(l<layersCount-1){
				activationsVector[l][layers[l]-1]=1;
			}
			
		}
	}
	
	~NeuralNetwork()
	{
		//TODO
	}
	
	void setWeights(float* weights){
		//transpose weights to multidemensional vector
		int16_t i=0;
		for(byte l=1;l<layersCount;l++){
			for(uint16_t j=0;j<layersN[l];j++){
				for(uint16_t k=0;k<layers[l-1];k++){
					weightsVector[l][j][k]=weights[i++];
				}
			}

		}
	}
	
	void randomize(){
		//TODO
	}
	
	void calculate(float* inputs, float* outputs, float* vectors=NULL){
		for(byte k=0;k<layersN[0];k++){
			activationsVector[0][k]=inputs[k];					//first i activations are inputs..
		}
			
		for(byte l=1;l<layersCount;l++){					//l=layer
			for(uint16_t j=0;j<layersN[l];j++){				//j=neuron
				sumsVector[l][j]=0;
				for(uint16_t k=0;k<layers[l-1];k++){
					sumsVector[l][j]+=weightsVector[l][j][k]*activationsVector[l-1][k];
				}
				activationsVector[l][j]=transferSigmoid(-sumsVector[l][j]);
			}
		}
		
		for(int i=0;i<layers[layersCount-1];i++){
			outputs[i]=activationsVector[layersCount-1][i];
		}
	}
	
	void backprop(float* inputs, float* targets){
		//TODO
	}
	
	static float transferSigmoid(float input){
		float output=1/(1+pow(M_E,-input));
		return output;
	}
	
	static float primeSigmoid(float input){
		float s=transferSigmoid(input);
		return s*(1-s);	//http://www.ai.mit.edu/courses/6.892/lecture8-html/sld015.htm
	}
};

#endif
