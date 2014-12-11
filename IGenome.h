//
//  IGenome.h
//  arbot
//
//  Created by Jernej Kaše on 12/11/14.
//  Copyright (c) 2014 Kase. All rights reserved.
//

#ifndef arbot_IGenome_h
#define arbot_IGenome_h

// we need this for qsort
class IGenome{
public:
	int32_t success; //for some reason gettr doesn't work in qsort method so we access member directly
	virtual int32_t getSuccess()=0;
	virtual byte getLength() const =0;
	virtual void listGenome(char* outStr, size_t strLen)=0;
	virtual void setSuccess(int32_t success)=0;
	virtual ~IGenome(){}

};

#endif
