/*
 *  Queue.cpp
 *  printer
 *
 *  Created by Brian Eschrich on 09.10.15
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "Presenter.h"
#include "GeneralPurposeFunctions.h"

namespace ofxModule {
    
	Presenter::Presenter(string moduleName):ModuleDrawable("Presenter",moduleName){
		
		// register mouse events for interaction
		ofRegisterMouseEvents(this);

		// loading the settings saved in settings.json/"Presenter"
		color = GeneralPurposeFunctions::colorFromJson(settings["color"]);

		ofBackground(0);
    }
  
    
    
    //------------------------------------------------------------------
    void Presenter::update() {
		// here could run something ;)
    }

	void Presenter::draw()
	{
		// draw the received text
		ofSetColor(color);
		ofDrawBitmapString(text, 50, 50);
	}

	void Presenter::mousePressed(ofMouseEventArgs & mouse)
	{
		// prepare a message with the mouse position
		ofJson msg;

		// set the function name
		msg["function"] = "multiply";

		// add a value
		msg["value"] = mouse.x;

		//send a message to Communicator
		notifyEvent(msg);
	}
    
    
    //------------------------------------------------------------------
    void Presenter::proceedModuleEvent(ModuleEvent& e) {
		
		// receive the communicator message of function name "response"
		if (e.message["function"] == "response") {

			// read the answer
			text = e.message["answer"].get<string>();
		}
    }
    
}
