#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	config = ofLoadJson("config.json");
	midiOut.listOutPorts();
	midiOut.openPort(config["port"].get<string>());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(0);
	vector<string> s{ "weis","rot","grün","blau","gelb","cyan","magenta" };
	int i = 0;
	for (auto c:s){
		ofDrawBitmapString(c + ofToString(i), 30, i * 10 + 10);
		++i;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	int channel = 1;
	switch (key)
	{
	case '0':
		
		break;
	case '1':
		midiOut.sendControlChange(channel, 15, 1);
		break;
	case '2':
		
		break;
	case '3':
		
		break;
	case '4':
		
		break;
	case '5':
		
		break;
	case '6':
		
		break;
	case '7':
		
		break;
	default:
		break;
	}
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

void ofApp::setColorScheme(int i)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
