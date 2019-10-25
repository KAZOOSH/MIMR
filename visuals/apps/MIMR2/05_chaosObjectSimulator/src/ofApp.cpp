#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	settings = ofLoadJson("settings.json");
	ofLogToConsole();

	live.setup(settings["ipAbleton"].get<string>());
	live.requestTempo();

	for (auto& object : settings["chaosObjects"]) {
		chaosObjects[object["id"]] = ChaosObject();
		chaosObjects[object["id"]].id = object["id"];
		chaosObjects[object["id"]].channel = object["channel"];
		chaosObjects[object["id"]].note = object["note"];
	}

	ofBackground(0);
	ofSetCircleResolution(900);

	midiOut.listOutPorts();
	midiOut.openPort(settings["midiPort"].get<string>());
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));

	live.requestTime();
	live.update();

	// calculate angle 
	float time = live.getTime();
	if (time != lastTime) {
		dsTimestamp = ofGetElapsedTimeMillis() - lastTimestamp;
		lastTimestamp = ofGetElapsedTimeMillis();
		dsTime = time - lastTime;
		lastTime = time;
	} else {
		if (live.isPlaying()) {
			auto ds = ofGetElapsedTimeMillis() - lastTimestamp;
			time = lastTime + ofMap(ds, 0, dsTimestamp, 0, dsTime,true);
		}
	}

	float bar = fmod(time, 4*settings["lengthRadarInBars"])/(4.0 * settings["lengthRadarInBars"]);
	lastAngle = currentAngle;
	currentAngle = bar;

	// send midi , first on then off
	for (auto& object : chaosObjects) {
		if (object.second.hasSend) {
			midiOut.sendControlChange(object.second.channel, object.second.note, 0);
			object.second.hasSend = false;
			cout << "midi off " << object.second.channel << "  " << object.second.note << endl;
		} else if (object.second.isActive){
			bool isSend = false;
			if (lastAngle > currentAngle && lastAngle > 0.95) {
				if ((object.second.angle < 1 && object.second.angle > lastAngle)
					|| (object.second.angle >= 0 && currentAngle >= object.second.angle)) {
					isSend = true;
				}
			}else if (lastAngle < object.second.angle && currentAngle >= object.second.angle) {
				isSend = true;
			}
			if (isSend) {
				object.second.hasSend = true;
				midiOut.sendControlChange(object.second.channel, object.second.note, 127);
				cout << "midi on " << object.second.channel << "  " << object.second.note << endl;
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofVec2f center = ofVec2f(ofGetWidth(), ofGetHeight())*.5;

	// draw circle
	ofSetColor(255);
	ofDrawCircle(center, ofGetHeight()*.5);
	ofSetColor(0);
	ofDrawCircle(center, ofGetHeight()*.5-2);

	// draw radar
	ofSetColor(255);
	ofDrawLine(center, center + ofVec2f(cos(currentAngle * 2 * PI)*ofGetHeight()*.5, sin(currentAngle * 2 * PI)*ofGetHeight()*.5));

	// draw objects
	for (auto& object : chaosObjects) {
		object.second.drawDebug();
	}

	ofSetColor(255,0,0);
	ofDrawBitmapString("mit 0-4 Objekt auswaehlen. Mit Mausklick platzieren. Ausserhalb des Kreises in Objekte inaktiv.", 20, ofGetHeight() -30);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{
	case '0': currentId = 0; break;
	case '1': currentId = 1; break;
	case '2': currentId = 2; break;
	case '3': currentId = 3; break;
	case '4': currentId = 4; break;
	default:
		currentId = 0;
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
	ofVec2f center = ofVec2f(ofGetWidth(), ofGetHeight())*.5;
	ofVec2f v0 = ofVec2f(1, 0);
	ofVec2f v1 = ofVec2f(x, y) - center;
	float angle = v0.angle(v1);
	if (angle < 0) angle = 360 + angle;
	angle /= 360.0;
	float radius = v1.length();

	chaosObjects[currentId].angle = angle;
	chaosObjects[currentId].radius = radius;

	if (radius < ofGetHeight()*.5) {
		chaosObjects[currentId].isActive = true;
	} else {
		chaosObjects[currentId].isActive = false;
	}
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

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
