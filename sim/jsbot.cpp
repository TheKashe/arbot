#ifndef ____JSBot__
#define ____JSBot__

//#define DEBUG 1

#include "Debug.h"

//"ported" arduino definitions
#include "arduino_port/Arduino.h"
#include "arduino_port/Serial.h"
#include "arduino_port/EEPROM.h"


//NaCl stuff
#include "/Volumes/DataHD/MacOs/jernej/Developer/nacl_sdk/pepper_38/include/ppapi/cpp/instance.h"
#include "/Volumes/DataHD/MacOs/jernej/Developer/nacl_sdk/pepper_38/include/ppapi/cpp/module.h"
#include "/Volumes/DataHD/MacOs/jernej/Developer/nacl_sdk/pepper_38/include/ppapi/cpp/var.h"


//arbot
#include "../common.h"
#include "randomseed.h"
#include "../EEPROMAnything.h"
#include "ArbotSimulatorPlatform.h"
#include "SimulatorController.h"



class JSBotInstance : public pp::Instance {
private:
	ArbotSimulatorPlatform *platform;
	SimulatorController *controller;
public:
	/// The constructor creates the plugin-side instance.
	/// @param[in] instance the handle to the browser-side plugin instance.
	explicit JSBotInstance(PP_Instance instance) : pp::Instance(instance){
		platform = new ArbotSimulatorPlatform(this);
		controller=new SimulatorController(platform); //sim controller has 150ms timer step
		Population::seedPopulation();
		controller->reset(600000); //let's use 10 minutes for sequence timeout
		
	}
	virtual ~JSBotInstance() {}
	
	/// Handler for messages coming in from the browser via postMessage().  The
	/// @a var_message can contain be any pp:Var type; for example int, string
	/// Array or Dictinary. Please see the pp:Var documentation for more details.
	/// @param[in] var_message The message posted by the browser.
	virtual void HandleMessage(const pp::Var& var_message) {
		// Ignore the message if it is not a string.
		if (!var_message.is_string())
			return;
		
		std::string message = var_message.AsString();
		
		if(message == "setup"){
			controller->setup();
			return;
		}
		
		if(message == "free"){
			platform->freedomInterrupt();
			return;
		}
		
		if(message == "collided"){
			platform->stalled();
			return;
		}
		
		if(message == "loop"){
			controller->loop();
			return;
		}
		
		
	}
};

/// The Module class.  The browser calls the CreateInstance() method to create
/// an instance of your NaCl module on the web page.  The browser creates a new
/// instance for each <embed> tag with type="application/x-pnacl".
class JSBotModule : public pp::Module {
public:
	JSBotModule() : pp::Module() {}
	virtual ~JSBotModule() {}
	
	/// @param[in] instance The browser-side instance.
	/// @return the plugin-side instance.
	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new JSBotInstance(instance);
	}
};

namespace pp {
	/// Factory function called by the browser when the module is first loaded.
	/// The browser keeps a singleton of this module.  It calls the
	/// CreateInstance() method on the object you return to make instances.  There
	/// is one instance per <embed> tag on the page.  This is the main binding
	/// point for your NaCl module with the browser.
	Module* CreateModule() {
		return new JSBotModule();
	}
}  // namespace pp

#endif // ____JSBot__