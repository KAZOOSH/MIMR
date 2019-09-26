#ifndef _ModuleLoader
#define _ModuleLoader

#include "ofMain.h"
#include "ModuleRunner.h"

namespace ofxModule {

	/// \brief loads module from "modules.json" configuration file
	class ModuleLoader {

	public:
		/// \brief the ModuleRunner needs a moduleLoader to load the Modules into
		ModuleLoader(ModuleRunner* moduleRunner);
		~ModuleLoader() {
		};

		bool configHasModules(string modulname);
		int getCountModulesConfig(string modulname);

		///\brief adds ModuleRunnable to app
		void addModule(ModuleRunnable* module);

		///\brief adds ModuleDrawable to app
		void addModule(ModuleDrawable* mdrawable);

		/// \brief registers events between classes
		void initModuleCommunication();


	private:

		ofJson config;

		//for building connections
		map<string, ModuleRunnable* > idMap;
		map< ModuleRunnable*, vector<string> > inputs;

		void addInput(ModuleRunnable* module, string input);
		void addInput(ModuleRunnable* module, vector<string> inputs);

		bool addToInputs(ModuleRunnable* module);
		void createInputList(ModuleRunnable* module);

		ModuleRunner* moduleRunner;
	};
}

#endif
