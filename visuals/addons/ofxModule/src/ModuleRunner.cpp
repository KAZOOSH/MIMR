/*
 *  ModuleRunner.cpp
 *  example_server_sharedcanvas
 *
 *  Created by Brian Eschrich on 29.07.15
 *  Copyright 2015 reddo UG. All rights reserved.
 *
 */

#include "ModuleRunner.h"
namespace ofxModule {


	void ModuleRunner::addModule(ModuleRunnable* module) {
		modules.push_back(module);

	}

	void ModuleRunner::addModule(ModuleDrawable* mdrawable) {
		modulesDrawable.push_back(mdrawable);
		modules.push_back(mdrawable);

	}

	void ModuleRunner::drawModules() {
		for (int i = 0; i < modulesDrawable.size(); ++i) {
			modulesDrawable[i]->draw();
		}
	}

	void ModuleRunner::updateModules()
	{
		for (auto& module : modules) {
			if (!module->getMultiThreaded() && !module->getIsIdle())
			{
				module->update();
			}
		}
	}

	void ModuleRunner::stopModules()
	{
		for (auto& module : modules) {
			if (!module->getMultiThreaded())
			{
				module->stopModule();
			}
			else {
				module->stopThread();
				module->stopModule();
			}
		}
	}

	void ModuleRunner::listModules()
	{
		if (modules.size() > 0) {
			cout << "moduleName\tmoduleClass\tisIdle\tisMultiThreaded" << endl;
			cout << "----------\t-----------\t------\t---------------" << endl;
			for (auto& module : modules)
			{
				cout << *module << endl;
			}
		}
		else {
			cout << "No active modules" << endl;
		}

	}

}