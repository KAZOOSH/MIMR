#include "Camera.h"

namespace ofxModule {
    //------------------------------------------------------------------
	Camera::Camera(string moduleName):ModuleRunnable("Camera", moduleName) {
        
		grabber.setup(640, 480);

		// set the shared pointer that streams the texture 
		texPtr = shared_ptr<ofTexture>(&tex);
    }
    
    //------------------------------------------------------------------
    void Camera::update() {
		grabber.update();
		if (grabber.isFrameNew()){

			// update the texture
			tex = grabber.getTexture();

			// send a new camera image
			notifyEvent(texPtr);
		}
    }
    
    //------------------------------------------------------------------
    void Camera::stopModule() {
        //what happens when the module will be stopped
    }

	// received module events will be processed here
	void Camera::proceedModuleEvent(ModuleEvent & e)
	{

	}
}
