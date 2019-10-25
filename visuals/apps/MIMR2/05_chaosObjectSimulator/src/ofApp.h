#pragma once

#include "ofMain.h"
#include "ofxAbletonLive.h"
#include "ofxMidi.h"
#include "ChaosObject.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
private:
	ofJson settings;
	ofxAbletonLive live;
	ofxMidiOut midiOut;

	map<int, ChaosObject> chaosObjects;

	int currentId = 0;

	// radar params
	float lastAngle = 0;
	float currentAngle = 0;

	long lastTimestamp = 0;
	long dsTimestamp = 0;
	float dsTime = 0;
	float lastTime = 0;
};
