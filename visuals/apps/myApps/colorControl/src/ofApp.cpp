#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	config = ofLoadJson("config.json");
	osc.setup(config["oscAddress"].get<string>(),config["oscPort"].get<int>());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(0);
	vector<string> s{ "weis","rot","grün","blau","gelb","cyan","magenta" };
	int i = 1;
	for (auto c:s){
		ofDrawBitmapString(c + " " + ofToString(i), 30, i * 10 + 10);
		++i;
	}

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	int channel = 1;
	switch (key)
	{
	case '1':
		setColorScheme(0);
		break;
	case '2':
		setColorScheme(1);
		break;
	case '3':
		setColorScheme(2);
		break;
	case '4':
		setColorScheme(3);
		break;
	case '5':
		setColorScheme(4);
		break;
	case '6':
		setColorScheme(5);
		break;
	case '7':
		setColorScheme(6);
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
	vector<int>			channels {   15,   25,45,46 };
	vector<vector<int>> colorSets{ {127, 127 ,0 ,127},
	{ 127, 127 ,0 ,127 },
	{ 127, 127 ,0 ,127 },
	{ 127, 127 ,0 ,127 },
	{ 127, 127 ,0 ,127 },
	{ 127, 127 ,0 ,127 },
	{ 127, 127 ,0 ,127 }
	};

	for (size_t c = 0; c < channels.size(); c++){

		ofxOscMessage m;
		m.setAddress("/mimr/color");
		m.addIntArg(176);
		m.addIntArg(channels[c]);
		m.addIntArg(colorSets[i][c]);
		osc.sendMessage(m, false);
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
