#include "ArbotVMA03Platform.h"
#include <IRremote.h> // no clue why, but if it's not included here, it doesn't compile 
#include "IrController.h"

//definitions for controller
#define IR_PIN         11      //where is the IR sensor connected
#define NO_COMMAND_TIMEOUT 250 //if we don't receive IR signal in this many cycles, 

//definitions for VMA03 controller, change those to match your pins!
#define PWM_A 3
#define PWM_B 9
#define DIR_A 4
#define DIR_B 8

IrController *controller=new IrController(new ArbotVMA03Platform(),IR_PIN,NO_COMMAND_TIMEOUT);

void setup()
{
  controller->setup();
}

void loop() {
  controller->loop();
}
