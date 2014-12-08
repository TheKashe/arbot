#ifndef EEPROM_h
#define EEPROM_h

#include <inttypes.h>

class EEPROMClass
{
private:
  byte _eeprom[1024];
public:
  uint8_t read(int i){
    return _eeprom[i];
  };

  void write(int i, uint8_t val){
    _eeprom[i]=val;
  };
};

EEPROMClass EEPROM;

#endif