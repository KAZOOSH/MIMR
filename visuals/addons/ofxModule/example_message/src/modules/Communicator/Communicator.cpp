#include "Communicator.h"

namespace ofxModule {

	// the Communicator runs in a seperate thread (set in the constructor)
	Communicator::Communicator(string moduleName):ModuleRunnable("Communicator", moduleName,true,true) {
		
		// loading the settings saved in settings.json/"Communicator"
		name = settings["name"].get<string>();
		multiplicator = settings["multiplicator"].get<float>();
    }
    
    //------------------------------------------------------------------
    void Communicator::update() {
       // here could run something ;)
    }
    
    //------------------------------------------------------------------
    void Communicator::stopModule() {
        //what happens when the module will be stopped
    }

	// received module events will be processed here
	void Communicator::proceedModuleEvent(ModuleEvent & e)
	{
		// check the function type
		if (e.message["function"] == "multiply") {

			// read the value
			float value = e.message["value"];

			// prepare a response
			ofJson response;

			// define the function
			response["function"] = "response";

			// define the value
			response["answer"] = name + " calculates " + ofToString(value) + " * " + ofToString(multiplicator) + " = " + ofToString(multiplicator*value);

			// spread the answer to all registered modules
			notifyEvent(response);
		}
	}
}
