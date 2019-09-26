#ifndef _Camera
#define _Camera

#include "ofMain.h"
#include "ModuleRunnable.h"


namespace ofxModule {

// A Module that that grabs the camera image and sends it as texture
class Camera : public ModuleRunnable{
	
  public:
	
	  Camera(string moduleName = "Camera");
    void update();
    void stopModule();
    
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
private:

	ofVideoGrabber grabber;
	ofTexture tex;
	shared_ptr<ofTexture> texPtr;
};
}
#endif
