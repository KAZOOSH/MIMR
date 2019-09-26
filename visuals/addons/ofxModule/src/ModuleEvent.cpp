#include "ModuleEvent.h"

namespace ofxModule {

	ModuleEvent::ModuleEvent(string moduleClass_, string id_,string function_ , ofJson message_)
	{
		moduleClass = moduleClass_;
		id = id_;
		message = message_;
		function = function_;
		type = MODULE_EVENT_MESSAGE;

		texture = nullptr;
		image = nullptr;
	}

	ModuleEvent::~ModuleEvent()
	{

	}


}