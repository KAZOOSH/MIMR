#ifndef _ModuleRunner
#define _ModuleRunner
#include "ModuleRunnable.h"
#include "ModuleDrawable.h"

namespace ofxModule {

	/// \brief start, stop and init modules
	class ModuleRunner {

	public:
		ModuleRunner() {};
		~ModuleRunner() {
		};

		void addModule(ModuleRunnable* module);
		void addModule(ModuleDrawable* mdrawable);

		/// \brief draws all ModuleDrawable
		void drawModules();

		/// \brief updates all Modules
		void updateModules();

		/// \brief stops all modules
		void stopModules();

		/// \brief lists all modules
		void listModules();

	private:
		vector< ModuleRunnable*> modules;
		vector< ModuleDrawable*> modulesDrawable;
	};

}

#endif
