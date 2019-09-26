#include "ModuleControl.h"

namespace ofxModule {
	//------------------------------------------------------------------
	ModuleControl::ModuleControl() {

		moduleRunner = new ModuleRunner();
		moduleLoader = new ModuleLoader(moduleRunner);
	}

	ModuleLoader* ModuleControl::getModuleLoader() {
		return moduleLoader;
	}

	ModuleRunner* ModuleControl::getModuleRunner() {
		return moduleRunner;
	}

}