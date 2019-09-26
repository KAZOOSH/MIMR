#include "ModuleLoader.h"

namespace ofxModule {

	ModuleLoader::ModuleLoader(ModuleRunner* moduleRunner_) {
		moduleRunner = moduleRunner_;
		config = ofLoadJson("modules.json");
	}

	bool ModuleLoader::configHasModules(string modulname)
	{
		return getCountModulesConfig(modulname) == 0 ? false : true;
	}

	int ModuleLoader::getCountModulesConfig(string modulname) {
		int ret = 0;
		for (auto& module : config) {
			if (module["type"].get<string>() == modulname) ++ret;
		}
		return ret;
	}

	void ModuleLoader::addModule(ModuleRunnable* module) {
		moduleRunner->addModule(module);
		addToInputs(module);
	}


	void ModuleLoader::addModule(ModuleDrawable* mdrawable) {
		moduleRunner->addModule(mdrawable);
		addToInputs(mdrawable);
	}

	bool ModuleLoader::addToInputs(ModuleRunnable* module) {
		idMap.insert(pair<string, ModuleRunnable*>(module->getModuleName(), module));

		createInputList(module);

		if (module->getModuleName() == module->getModuleClass()) ofLogNotice("ModuleLoader", "Module: " + module->getModuleName() + " added");
		else ofLogNotice("ModuleLoader", "Module: " + module->getModuleName() + " of class " + module->getModuleClass() + " added");
		return true;
	}

	void ModuleLoader::initModuleCommunication() {
		if (!inputs.empty()) {
			for (auto& input : inputs)
			{
				for (int i = 0; i < input.second.size(); ++i) {
					if (idMap.find(input.second[i]) != idMap.end()) {
						input.first->registerModuleEvent(idMap[input.second[i]]);
						ofLogNotice("ModuleLoader::createModuleConnections", input.first->getModuleName() + " <- " + input.second[i]);
					}
					else {
						ofLogNotice("ModuleLoader::createModuleConnections", input.first->getModuleName() + " : no connection found for -> " + input.second[i]);
					}
				}

			}
			for (auto& input : inputs)
			{
				input.first->onAllModulesLoaded();
			}
		}

	}


	void ModuleLoader::addInput(ModuleRunnable* module, string input)
	{
		vector<string> s;
		s.push_back(input);
		addInput(module, s);
	}
	void ModuleLoader::addInput(ModuleRunnable* module, vector<string> inputs_)
	{
		inputs.insert(pair< ModuleRunnable*, vector<string> >(module, inputs_));
	}

	void ModuleLoader::createInputList(ModuleRunnable* module) {

		vector<string> inputList;
		for (auto& jmodule : config) {
			if (jmodule["type"] == module->getModuleName()) {
				if (jmodule["inputs"] != nullptr && jmodule["inputs"].size() > 0) {
					for (auto& input : jmodule["inputs"]) {
						inputList.push_back(input);
					}
				}
			}
		}

		if (inputList.size() > 0) {
			addInput(module, inputList);
		}
		else {
			ofLogNotice("ModuleLoader::createInputList", "no con for " + module->getModuleName());
		}
	}

}
