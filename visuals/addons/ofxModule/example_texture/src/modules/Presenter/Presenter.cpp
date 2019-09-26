#include "Presenter.h"
#include "GeneralPurposeFunctions.h"

namespace ofxModule {
    
	Presenter::Presenter(string moduleName):ModuleDrawable("Presenter",moduleName){
		
		// register mouse events for interaction
		ofRegisterMouseEvents(this);


		ofBackground(0);
    }
  
    
    
    //------------------------------------------------------------------
    void Presenter::update() {
		// here could run something ;)
    }

	void Presenter::draw()
	{
		// draw the camera image
		if (camImage != nullptr) camImage->draw(0, 0, 640, 480);

		ofDrawBitmapStringHighlight("stop capturing by holding mouse button", 50, 50);
	}

	void Presenter::mousePressed(ofMouseEventArgs & mouse)
	{
		// stop recording with sending an idle message to the camera
		ofJson msg;
		msg["function"] = "setIdle";
		msg["value"] = true;
		notifyEvent(msg);
	}

	void Presenter::mouseReleased(ofMouseEventArgs & mouse)
	{
		// continue recording with sending an unidle message to the camera
		ofJson msg;
		msg["function"] = "setIdle";
		msg["value"] = false;
		notifyEvent(msg);
	}
    
    
    //------------------------------------------------------------------
    void Presenter::proceedModuleEvent(ModuleEvent& e) {
		
		// update the cam image 
		if (e.type == ModuleEvent::MODULE_EVENT_TEXTURE) {
			camImage = e.texture;
		}
    }
    
}
