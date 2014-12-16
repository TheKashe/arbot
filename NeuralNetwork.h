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

typedef struct{
	float activation;					// there are a many as neurons + bias neurons
	float error;						// as many as neurons
	float *weights;						// multiple per neuron
} Neuron;

class NeuralNetwork{
private:
	uint8_t		layersCount;
	Neuron**	neurons;
	uint16_t*	layersN;
	uint16_t*	layers;
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
		neurons					= new Neuron*[layersCount];
		for(byte l=0;l<layersCount;l++){
			neurons[l] = new Neuron[l==layersCount-1?layersN[l]:layers[l]];
			uint16_t j = 0;
			for(j=0;j<layersN[l];j++){
				//inputs don't have weights
				if(l>0){
					neurons[l][j].weights=new float[layers[l-1]];
				}
			}
			//bias neuron for all but last layers
			if(l==layersCount-1) continue;
			neurons[l][j].activation = 1;
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
					neurons[l][j].weights[k]=weights[i++];
				}
			}

		}
	}
	
	void randomize(){
		for(byte l=1;l<layersCount;l++){					//l=layer
			for(uint16_t j=0;j<layersN[l];j++){				//j=neuron
				for(uint16_t k=0;k<layers[l-1];k++){
					neurons[l][j].weights[k]=RND*2-1;
				}
			}
		}
	}
	
	void calculate(float* inputs, float* outputs){
		for(uint16_t j=0;j<layersN[0];j++){
			neurons[0][j].activation=inputs[j];
		}
			
		for(byte l=1;l<layersCount;l++){					//l=layer
			for(uint16_t j=0;j<layersN[l];j++){				//j=neuron
				Neuron* n=&neurons[l][j];
				float weightedInput=0;
				for(uint16_t k=0;k<layers[l-1];k++){
					weightedInput+=n->weights[k]*neurons[l-1][k].activation;
				}
				n->activation=transferSigmoid(weightedInput);
			}
		}
		
		for(uint16_t i=0;i<layersN[layersCount-1];i++){
			outputs[i]=neurons[layersCount-1][i].activation;
		}
	}
	
	
	void backprop(float* targets, float learningRate){
		//1. compute error of the output layer
		byte l=layersCount-1;
		for(uint16_t j=0;j<layersN[l];j++){
			neurons[l][j].error =  targets[j]-neurons[l][j].activation;			//E'=(t-y)
			//FANN does sth in between
			//supposedly this is a must for more than 1 output value
			//neuron_diff = (fann_type) log((1.0 + neuron_diff) / (1.0 - neuron_diff));
			neurons[l][j].error=log((1.0 + neurons[l][j].error) / (1.0 - neurons[l][j].error));
			neurons[l][j].error *= derivativeSigmoid(neurons[l][j].activation);
			/*for(uint16_t k=0;k<layers[l-1];k++){
				neurons[l][j].weights[k]+=learningRate*neurons[l][j].error*neurons[l][j].weightedInput;
			}*/
		}
		//2. backprop + update weights
		for(;l>0;l--){										//l je layer
			for(uint16_t j=0;j<layersN[l];j++){				//j = neuron
				if(l!=layersCount-1){
					//backprop weights
					neurons[l][j].error=0;
					for(uint16_t d=0;d<layersN[l+1];d++){		//d = neuron on a downstream level (closer to output)
						neurons[l][j].error+=neurons[l+1][d].error*neurons[l+1][d].weights[j];
					}
					neurons[l][j].error *= derivativeSigmoid(neurons[l][j].activation);
				}
				//recalculate weights for hidden layers			//u = upstream neuron, closer to input
				for(uint16_t u=0;u<layers[l-1];u++){
					neurons[l][j].weights[u] += neurons[l][j].error*neurons[l-1][u].activation*learningRate;
				}
			}
		}
		
		//3.update weights
	}
	
	static float transferSigmoid(float input){
		float output=1/(1+pow(M_E,-input*2));
		return output;
	}
	
	//because neuron value already IS sigmoid, just use input to get the derrivative
	static float derivativeSigmoid(float input){
		//input needs to be clipped, or we'll converge towards 1/0
		input=input<0.01?0.1:input>0.99?0.99:input;
		return 2*input*(1-input);	//http://www.ai.mit.edu/courses/6.892/lecture8-html/sld015.htm
	}
};

#endif
