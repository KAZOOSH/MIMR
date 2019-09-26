/*
 *  Queue.cpp
 *  printer
 *
 *  Created by Brian Eschrich on 09.10.15
 *  Copyright 2015 __MyCompanyName__. All rights reserved.
 *
 */

#include "Visualization.h"
#include "GeneralPurposeFunctions.h"

namespace ofxModule {
    
	Visualization::Visualization(string moduleName):ModuleDrawable("Visualization",moduleName){
		
		// register mouse events for interaction
		ofRegisterMouseEvents(this);

		// loading the settings saved in settings.json/"Presenter"
		color = GeneralPurposeFunctions::colorFromJson(settings["color"]);

		ofBackground(0);
    }
  
    
    
    //------------------------------------------------------------------
    void Visualization::update() {
		// here could run something ;)
    }

	void Visualization::draw()
	{
		// draw the received text
		ofSetColor(color);
		ofDrawBitmapString(text, 50, 50);
	}

	void Visualization::mousePressed(ofMouseEventArgs & mouse)
	{
		// prepare a message with the mouse position
		ofJson msg;

		// add a value
		msg["value"] = mouse.x;

		//send a message to Communicator
		notifyEvent("multiply",msg);
	}
    
    
    //------------------------------------------------------------------
    void Visualization::proceedModuleEvent(ModuleEvent& e) {
		
		// receive the communicator message of function name "response"
		if (e.function == "response") {

			// read the answer
			text = e.message["answer"].get<string>();
		}
    }
    
}
