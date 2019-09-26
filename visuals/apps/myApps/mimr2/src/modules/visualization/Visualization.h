#pragma once

#include "ofMain.h"
#include "ModuleDrawable.h"

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

    
	
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
    private:
		ofColor color;
		string text = "Click somewhere to multiply the X-position of the mouse.";
};
}
