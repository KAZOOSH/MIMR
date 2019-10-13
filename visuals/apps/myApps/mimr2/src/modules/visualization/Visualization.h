#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ModuleDrawable.h"
#include "MidiCtrl.h"
#include "RadarAttributes.h"
#include "Renderer.h"


namespace ofxModule {
	// Basic example of a ModuleDrawable with communication
class Visualization : public ModuleDrawable{
	
  public:
	
	  Visualization(string moduleName = "Visualization");
	
    void update();
	void draw();

	void mouseMoved(ofMouseEventArgs & mouse) {};
	void mouseDragged(ofMouseEventArgs & mouse) {};
	void mousePressed(ofMouseEventArgs & mouse);
	void mouseReleased(ofMouseEventArgs & mouse) {};
	void mouseScrolled(ofMouseEventArgs & mouse) {};
	void mouseEntered(ofMouseEventArgs & mouse) {};
	void mouseExited(ofMouseEventArgs & mouse) {};

	void keyPressed(ofKeyEventArgs & args);
	void keyReleased(ofKeyEventArgs & args) {};
    
	
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
    private:
		ofxPanel gui;
		MidiCtrl midiCtrl;
		RadarAttributes radarAttributes;
		Renderer renderer;
		ofFbo rendering;

		

};
}
