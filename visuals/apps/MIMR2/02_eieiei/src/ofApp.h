#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxMidi.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp , public ofxMidiListener{
public:
    
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
	void newMidiMessage(ofxMidiMessage& eventArgs);
    
    ofShader shader;
    ofPlanePrimitive plane;
    ofImage img;
    int depth;
    
    ofxPanel gui;
    ofParameter<float> time;
    
    
    ofFbo fbo;
    
    int n;
	ofxOscReceiver osc;
	//ofxMidiIn midiIn;

	float zoom = 0;
	float brightness = 0.3;
	ofJson settings;
};