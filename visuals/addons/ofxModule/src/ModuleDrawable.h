#ifndef _ModuleDrawable
#define _ModuleDrawable

#include "ofMain.h"
#include "ModuleRunnable.h"

namespace ofxModule {


	/// \brief a module with a seperate drwaing functions
	///
	/// since pure multithreaded modules cannot use the openGL thread for drawing using FBOs etc.,
	/// this class has seperate draw() function that always runs in the main thread 
	class ModuleDrawable : public ModuleRunnable {

	public:

		ModuleDrawable(string moduleClass, string moduleName, bool isMultiThreaded = false, bool isStartThread = false, int fps = 60) :ModuleRunnable(moduleClass, moduleName, isMultiThreaded, isStartThread, fps) {};
		ModuleDrawable(string moduleClass, string moduleName,string settingsPath, bool isMultiThreaded = false, bool isStartThread = false, int fps = 60) :ModuleRunnable(moduleClass, moduleName,settingsPath, isMultiThreaded, isStartThread, fps) {};
		/// \brief draw function that always runs in the main thread
		virtual void draw() = 0;

	};

}

#endif
