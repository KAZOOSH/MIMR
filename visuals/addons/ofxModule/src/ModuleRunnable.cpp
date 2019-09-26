#include "ModuleRunnable.h"

namespace ofxModule {
	ModuleRunnable::ModuleRunnable(string moduleClass, string moduleName, bool isMultiThreaded, bool isStartThread, int fps)
	{
		initModule(moduleClass, moduleName, "settings.json/" + moduleName, isMultiThreaded, isStartThread, fps);
	}

	ModuleRunnable::ModuleRunnable(string moduleClass, string moduleName, string settingsPath, bool isMultiThreaded, bool isStartThread, int fps)
	{
		initModule(moduleClass, moduleName, settingsPath, isMultiThreaded, isStartThread, fps);
	}

	ModuleRunnable::~ModuleRunnable()
	{
		if (isMultiThreaded) {
			waitForThread(true);
		}
	}

	void ModuleRunnable::initModule(string moduleClass_, string moduleName_,string settingsPath_, bool isMultiThreaded_, bool isStartThread, int fps_) {
		fps = fps_;
		moduleClass = moduleClass_;
		moduleName = moduleName_;
		settingsPath = settingsPath_;
		loadSettings();
		isIdle = false;
		isMultiThreaded = isMultiThreaded_;
		if (isMultiThreaded && isStartThread) startThread();
	}


	bool ModuleRunnable::getMultiThreaded()
	{
		return isMultiThreaded;
	}

	void ModuleRunnable::threadedFunction() {
		while (isThreadRunning())
		{
			sleep(1000 / fps);
			if (lock())
			{
				update();
				unlock();
			}
			else
			{
				ofLogWarning("ModuleRunnable::threadedFunction()") << "Unable to lock mutex of " + moduleName;
				sleep(1000);
			}
		}

	}

	void ModuleRunnable::onModuleEvent(ModuleEvent &e) {
		if ((e.destClass == "" && e.destId == "") || e.destId == moduleName || e.destClass == moduleClass) {
			if (e.function == "setIdle") {
				if (e.message["value"].get<bool>() == true) setIsIdle(true);
				else setIsIdle(false);
			}
			proceedModuleEvent(e);
		}
	}

	void ModuleRunnable::registerModuleEvent(ModuleRunnable* module) {
		if (lock())
		{
			ofAddListener(module->moduleEvent, this, &ModuleRunnable::onModuleEvent);
			unlock();
		}
		else
		{
			ofLogWarning("ModuleRunnable::newModuleEvent()") << "Unable to lock mutex.";
			sleep(1000);
		}
	}

	string ModuleRunnable::getModuleClass() {
		return moduleClass;
	}

	string ModuleRunnable::getModuleName() {
		return moduleName;
	}

	void ModuleRunnable::notifyEvent(shared_ptr<ofFbo> fbo, string function, ofJson message) {
		if (!isIdle) {
			ModuleEvent e = ModuleEvent(moduleClass, moduleName,function, message);
			e.fbo = fbo;
			e.type = ModuleEvent::MODULE_EVENT_FBO;
			ofNotifyEvent(moduleEvent, e);
		}
	}

	void ModuleRunnable::notifyEvent(shared_ptr<ofTexture> texture, string function, ofJson message) {
		if (!isIdle) {
			ModuleEvent e = ModuleEvent(moduleClass, moduleName, function, message);
			e.texture = texture;
			e.type = ModuleEvent::MODULE_EVENT_TEXTURE;
			ofNotifyEvent(moduleEvent, e);
		}
	}

	void ModuleRunnable::notifyEvent(shared_ptr<ofImage> image, string function, ofJson message) {
		if (!isIdle) {
			ModuleEvent e = ModuleEvent(moduleClass, moduleName, function, message);
			e.image = image;
			e.type = ModuleEvent::MODULE_EVENT_IMAGE;
			ofNotifyEvent(moduleEvent, e);
		}
	}

	void ModuleRunnable::notifyEvent(shared_ptr<ofPixels> pixels, string function, ofJson message) {
		if (!isIdle) {
			ModuleEvent e = ModuleEvent(moduleClass, moduleName, function, message);
			e.pixels = pixels;
			e.type = ModuleEvent::MODULE_EVENT_PIXELS;
			ofNotifyEvent(moduleEvent, e);
		}
	}

	void ModuleRunnable::notifyEvent(string function, ofJson message) {
		if (!isIdle) {
			ModuleEvent e = ModuleEvent(moduleClass, moduleName,function, message);
			ofNotifyEvent(moduleEvent, e);
		}
	}

	bool ModuleRunnable::getIsIdle() {
		return isIdle;
	}
	void ModuleRunnable::setIsIdle(bool isIdle_) {
		isIdle = isIdle_;
	}

	void ModuleRunnable::setSettingsPath(string path)
	{
		settingsPath = path;
	}

	string ModuleRunnable::getSettingsPath()
	{
		return settingsPath;
	}

	void ModuleRunnable::loadSettings() {
		loadSettings(settingsPath);
	}

	void ModuleRunnable::saveSettings() {
		saveSettings(settingsPath);

	}
	void ofxModule::ModuleRunnable::loadSettings(string path)
	{
		// split file path and json path
		auto pathVar = ofSplitString(path, ".json");

		// load file
		ofJson sTemp = ofLoadJson(pathVar[0] + ".json");

		if (pathVar.size() >= 2) {
			// get the correct node in the json tree
			auto jsonPath = ofSplitString(pathVar[1], "/");
			for (size_t i = 0; i < jsonPath.size(); i++) {
				if (jsonPath[i] != "") sTemp = sTemp[jsonPath[i]];
			}
		}

		//set settings
		settings = sTemp;
		if (settings != nullptr) ofLogNotice("ModuleRunnable::loadSettings", moduleName + " settings loaded");
		else {
			ofLogNotice("ModuleRunnable::loadSettings", "No settings preset for " + moduleName);
			settings = ofJson();
		}
	}

	void ModuleRunnable::saveSettings(string path)
	{
		// split file path and json path
		auto pathVar = ofSplitString(path, ".json");

		// load file
		ofJson sSave = ofLoadJson(pathVar[0] + ".json");
		ofJson* sPointer = &sSave;

		if (pathVar.size() >= 2) {
			// get the correct node in the json tree
			auto jsonPath = ofSplitString(pathVar[1], "/");
			for (size_t i = 0; i < jsonPath.size(); i++) {
				if (jsonPath[i] != "") sPointer = &sSave[jsonPath[i]];
			}
		}

		*sPointer = settings;
		ofSavePrettyJson(pathVar[0] + ".json", sSave);
	}


}
