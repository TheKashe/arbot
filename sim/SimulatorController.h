#ifndef ____SimulatorController__
#define ____SimulatorController__

#include "../FreedomController.h"
#include <unistd.h>

class SimulatorController: public FreedomController{
public:
	SimulatorController(IArbotVMA03MDPlatform *arbotPlatform):FreedomController(arbotPlatform){
	}
	
	virtual void setup(){
		//here we jump over FreedomController setup intentionally!
	    ArbotController::setup();
	}
	
	virtual void setStateAfterGenomeSwitch(){
		sleep(1);
		state=STARTING; //note, we will need hard reset to restart
	}
	
};

#endif  /*____SimulatorController__*/