//
//  EEPROMAnything.h
//  arbot
//
//  Created by Jernej Kaše on 12/5/14.
//  Copyright (c) 2014 Kase. All rights reserved.
//

#ifndef arbot_EEPROMAnything_h
#define arbot_EEPROMAnything_h

//because we are porting this to sim where we need different includes,
//these need to be included in the top file
//#include <EEPROM.h>
//#include <Arduino.h>  // for type definitions

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);
    return i;
}

#endif
