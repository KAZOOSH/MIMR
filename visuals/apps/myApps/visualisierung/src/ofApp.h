#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "ofxGui.h"
#include "Layer.h"
#include "GoldenBox.h"
#include "Kuehler.h"
#include "Radar.h"
#include "Universum.h"
#include "Foehn.h"
#include "Kurbel.h"
#include "ofxOscParameterSync.h"

class ofApp : public ofBaseApp, public ofxMidiListener{
public:
    
    void setup();
	void setupGui();
    void update();
    void draw();
	void drawGui(ofEventArgs & args);
    void exit();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void onSceneChanged(int& nScene);
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    
    void initMidi();

	ofxOscParameterSync sync;
    
private:
    ofxPanel gui;
    ofParameterGroup p;
    
    ofParameter<string> midiPort;
    ofxMidiIn midiIn;
    ofxMidiOut midiOut;
    
    vector<Layer*> layer;
    
	bool isGuiVisible;
    
    RadarAttributes params;
    
    ofFbo fbo;
    float rotationFbo;
    

    
};
