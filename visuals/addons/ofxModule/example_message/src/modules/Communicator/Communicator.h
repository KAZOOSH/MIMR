#ifndef _Communicator
#define _Communicator

#include "ofMain.h"
#include "ModuleRunnable.h"


namespace ofxModule {

// Basic example of a ModuleRunnable with communication
class Communicator : public ModuleRunnable{
	
  public:
	
	Communicator(string moduleName = "Communicator");
    void update();
    void stopModule();
    
protected:
    void proceedModuleEvent(ModuleEvent& e);
    
private:

	string name;
	float multiplicator;
    
};
}
#endif
