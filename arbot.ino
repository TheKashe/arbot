#include "ArbotVMA03MDPlatform.h"
#include "FreedomController.h"

#include <EEPROM.h>

//definitions for controller
#define IR_PIN         11      //where is the IR sensor connected
#define NO_COMMAND_TIMEOUT 250 //if we don't receive IR signal in this many cycles, 

//definitions for VMA03 controller, change those to match your pins!
#define PWM_A 3
#define PWM_B 9
#define DIR_A 4
#define DIR_B 8

ArbotController *controller=new FreedomController(new ArbotVMA03MDPlatform(PWM_A,PWM_B,DIR_A,DIR_B));

void setup(){
  Serial.begin(9600);
  controller->setup();
}

void loop() {
  controller->loop();
}
