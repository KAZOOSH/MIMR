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

class ofApp : public ofBaseApp, public ofxMidiListener{
public:
    
    void setup();
    void update();
    void draw();
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
    
    void newMidiMessage(ofxMidiMessage& eventArgs);
    
    
    void initMidi();
    
private:
    ofxPanel gui;
    ofParameterGroup p;
    
    ofParameter<string> midiPort;
    ofxMidiIn midiIn;
    
    vector<Layer*> layer;
    
    
    
    RadarAttributes params;
    
    ofFbo fbo;
    float rotationFbo;
    
    
    //midi
    
};
