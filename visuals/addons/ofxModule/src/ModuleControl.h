#ifndef _ofxModule
#define _ofxModule

#include "GeneralPurposeFunctions.h"
#include "ModuleRunnable.h"
#include "ModuleRunner.h"
#include "ModuleLoader.h"

namespace ofxModule {

	/// \brief the control of the ofxModule system
	class ModuleControl {

	public:

		ModuleControl();

		ModuleLoader*   getModuleLoader();
		ModuleRunner*  getModuleRunner();

	private:
		ModuleLoader*   moduleLoader;
		ModuleRunner*   moduleRunner;

	};

}

#endif

