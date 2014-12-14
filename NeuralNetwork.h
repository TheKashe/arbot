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

class NeuralNetwork{
private:
	uint8_t layersCount;
	uint16_t activationsCount=0;
	uint16_t weightsCount=1;
	uint16_t biasesCount=0;
	
	uint16_t* layers;
	float *activations;
	float *weights;
	float *biases;
public:
	NeuralNetwork(byte layerCount, ...)
	{
		layersCount			= layerCount;
		activationsCount	= 0;
		weightsCount		= 0;
		biasesCount			= 0;
		
		layers=new uint16_t[layerCount];
		
		va_list param;
		va_start(param, layerCount);
		for (byte i = 0; i < layerCount; i++){
			layers[i]=va_arg (param, uint16_t);
			activationsCount+=layers[i];
			if(i>0){
				weightsCount+=layers[i]*layers[i-1];
				biasesCount+=layers[i];
			}
		}
		va_end (param);
		
		activations	= new float[activationsCount];
		weights		= new float[weightsCount];
		biases		= new float[biasesCount];
	}
	
	~NeuralNetwork()
	{
		delete[] layers;
		delete[] activations;
		delete[] weights;
		delete[] biases;
	}
	
	void setWeights(float* weights){
		for(uint16_t i=0;i<weightsCount;i++){
			this->weights[i]=weights[i];
		}
		std::cout << "weights:\n";
		for(uint16_t i=0;i<weightsCount;i++){
			std::cout<<weights[i] << "\n";
		}
	}
	
	void setBiases(float* biases){
		for(uint16_t i=0;i<biasesCount;i++){
			this->biases[i]=biases[i];
		}
		std::cout << "biases:\n";
		for(uint16_t i=0;i<biasesCount;i++){
			//biases[i]=RND*10-5;
			std::cout<<biases[i] << "\n";
		}
	}
	
	void calculate(float* inputs, float* outputs){
		for(byte i=0;i<layers[0];i++){
			activations[i]=inputs[i];					//first i activations are inputs..
		}
		uint16_t nIdx=layers[0];							//nIdx holds current neuron index
		uint16_t bIdx=0;									//bias index
		uint16_t wIdx=0;									//which is the current weight we are processing
		uint16_t inOffset=0;								//offset of input in activation array
		
		
		for(byte i=1;i<layersCount;i++){				//i will run over layers, starting at 2nd, first layer is input only so no weights and biases apply...
			for(uint16_t j=0;j<layers[i];j++){			//j will run over nerons in a layer
				activations[nIdx]=0;					//clear old values
				for(uint16_t k=0; k<layers[i-1];k++){	//k will run over inputs per neuron, previous layer tells us how many inputs we have per neuron
					activations[nIdx]+=weights[wIdx++]*activations[k+inOffset];
				}
				activations[nIdx]+=biases[bIdx++];
				activations[nIdx]=transferSigmoid(activations[nIdx]);
				nIdx++;
			}
			inOffset+=layers[i-1]; //input offset is increment by number of
		}
		
		uint16_t k=0;
		for(int i=activationsCount-layers[layersCount-1];i<activationsCount;i++){
			outputs[k++]=activations[i];
		}
	}
	
	static float transferSigmoid(float input){
		float output=1/(1+pow(M_E,-input));
		return output;
	}
};

#endif
