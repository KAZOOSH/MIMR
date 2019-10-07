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
		
		ofSetWindowShape(1920, 1080);
		// register mouse events for interaction
		ofRegisterMouseEvents(this);
		ofRegisterKeyEvents(this);

		radarAttributes.setup(settings["midi"]["instruments"], settings["rendering"]);

		MidiCtrlSettings midiSettings;
		midiSettings.jsonSettings = settings["midi"];
		midiSettings.radarAttributes = shared_ptr<RadarAttributes>(&radarAttributes);
		midiCtrl.setup(midiSettings);

		gui.setup("params");
		gui.add(radarAttributes.params);

		RendererSettings rendererSettings;
		rendererSettings.radar = shared_ptr<RadarAttributes>(&radarAttributes);
		rendererSettings.settings = settings;
		renderer.setup(rendererSettings);

		ofBackground(0);

		rendering.allocate(settings["rendering"]["radarSize"][0], settings["rendering"]["radarSize"][1]);
    }
  
    
    
    //------------------------------------------------------------------
    void Visualization::update() {
		midiCtrl.update();

		//cout << radarAttributes.beat.getCurrentBeat().angle << " " << radarAttributes.beat.getCurrentBeat().beat << endl;
    }

	void Visualization::draw()
	{
		rendering.begin();
		ofClear(0, 0);
		renderer.draw();
		rendering.end();
		rendering.draw((ofGetWidth()-rendering.getWidth())*0.5, 0);
		gui.draw();
	}

	void Visualization::mousePressed(ofMouseEventArgs & mouse)
	{

	}
    
    
	void Visualization::keyPressed(ofKeyEventArgs & args)
	{
		if (args.key == 'l') {
			renderer.loadShaders();
		}
	}

	//------------------------------------------------------------------
    void Visualization::proceedModuleEvent(ModuleEvent& e) {
		

    }
    
}
