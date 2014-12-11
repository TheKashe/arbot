//
//  Neuron.h
//  arbot
//
//  Created by Jernej Kaše on 12/10/14.
//  Copyright (c) 2014 Kase. All rights reserved.
//

#ifndef arbot_Neuron_h
#define arbot_Neuron_h

class Neuron{
private:
	float*	inputs;
	float*	weights;
	byte	inputsCount;
	byte	receiversCount;
	byte	currentInput;
	byte	currentWeight;
	float	output;
	Neuron** receivers; //array of receiving neurons
public:
	Neuron(byte inputsCount, byte receiversCount)
	{
		init(inputsCount,receiversCount);
	}
	
	Neuron(){
	}
	
	void init(byte inputsCount, byte receiversCount)
	{
		this->inputsCount		= inputsCount;
		this->receiversCount	= receiversCount;
		this->currentInput		= 0;
		this->currentWeight		= 0;
		inputs=new float[inputsCount];
		weights=new float[inputsCount];
		receivers=new Neuron*[receiversCount];
	}
	
	virtual ~Neuron()
	{
		delete inputs;
		delete weights;
		delete[] receivers;
	}
	
	
	void setReceiver(byte index, Neuron* neuron)
	{
		receivers[index]=neuron;
	}
	
	
	void setWeight(byte index, float value)
	{
		weights[index]=value;
	}
	
	void setNextWeight(float value){
		setWeight(currentWeight, value);
		currentWeight++;
	}
	
	void setInput(byte index, float value)
	{
		inputs[index]=value*weights[index];
		output+=inputs[index];
	}
	
	
	void setNextInput(float value){
		setInput(currentInput, value);
		currentInput++;
	}
	
	
	float getOutput()
	{
		return output;
	}
	
	
	void calculateAndPropagate(){
		calculate();
		propagate();
	}
	
	
	void calculate()
	{
		output=transferLinear(output);
	}
	
	void reset()
	{
		output			= 0;
		currentInput	= 0;
		currentWeight	= 0;
		for (byte j = 0; j < inputsCount; j++) {
			inputs[j]=0;
		}
		for(int i=0;i<receiversCount;i++){
			receivers[i]->reset();
		}
	}
	
	byte getWeightCount()
	{
		return inputsCount;
	}
private:
	
	/*void _init(byte inputsCount, byte receiversCount, va_list valist)
	{
		this->inputsCount=inputsCount;
		this->receiversCount=receiversCount;
		this->currentInput=0;
		inputs=new float[inputsCount];
		weights=new float[inputsCount];
		receivers=new Neuron*[receiversCount];
		
		for (byte j = 0; j < inputsCount; j++) {
			double x=va_arg(valist, double); //variadic input is expanded to double...
			weights[j]= (float)x;
		}
	}*/
	
	void propagate()
	{
		for(int i=0;i<receiversCount;i++)
		{
			receivers[i]->setNextInput(output);
		}
	}
	
/*------------------------------------------------------------------------------------------*/
/*	TRANSFER FUNCTIONS																		*/
/*------------------------------------------------------------------------------------------*/
public:
	static float transferLinear(float input){
		float slope=1.0;
		return input*slope;
	}
	
	static float transferLinear10(float input){
		float slope=10.0;
		return input*slope;
	}
};

#endif
