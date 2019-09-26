#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    

	ofSetLogLevel(OF_LOG_NOTICE);
	ofLogToConsole();

	// Our app will consist of 2 modules that show how module communication works
	// Communicator : it receives a number, multiplies it and sends it to the presenter
	// Presenter : it sends a number, receives an answer from the communicator and shows ist on the screen
    
    // first we init the app by crawling the config (modules.json), 
	// searching for the specific components and adding them to the module control
    
    if (webModule.getModuleLoader()->getCountModulesConfig("Communicator")) {
		
		// create a communicator
        auto mod = new Communicator();

		// add the module to the control
        webModule.getModuleLoader()->addModule(mod);
    }
	if (webModule.getModuleLoader()->getCountModulesConfig("Presenter")) {
		auto mod = new Presenter();
		webModule.getModuleLoader()->addModule(mod);
	}
    
	// enables the communication between modules (set in modules.json)
	webModule.getModuleLoader()->initModuleCommunication();
}

//--------------------------------------------------------------
void ofApp::update(){
	// update the modules
	webModule.getModuleRunner()->updateModules();
}

//--------------------------------------------------------------
void ofApp::draw(){
	// draw the modules
    webModule.getModuleRunner()->drawModules();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
