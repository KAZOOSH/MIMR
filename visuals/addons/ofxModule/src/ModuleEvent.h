#ifndef _ModuleEvent
#define _ModuleEvent

#include "ofMain.h"


/// \brief event class for communication between modules
/// the event class provides 4 types : messages, images, textures or fbos

namespace ofxModule {
	class ModuleEvent {

	public:
		enum EventType {
			MODULE_EVENT_MESSAGE,
			MODULE_EVENT_TEXTURE,
			MODULE_EVENT_IMAGE,
			MODULE_EVENT_FBO,
			MODULE_EVENT_PIXELS
		};

		ModuleEvent(string moduleClass, string id, string function, ofJson message);
		~ModuleEvent();

		string function = "";
		string moduleClass;
		string id;
		
		string destClass = "";
		string destId = "";

		ofJson message;
		shared_ptr<ofTexture> texture;
		shared_ptr<ofImage> image;
		shared_ptr<ofFbo> fbo;
		shared_ptr<ofPixels> pixels;

		EventType type;
	};
}

#endif
