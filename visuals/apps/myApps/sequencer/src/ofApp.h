#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMidi.h"
#include "ofxSequencer.h"
#include "Button.h"

typedef union midiMessage {
    std::int32_t intValue;
    char message[4];
};


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
    
    void onBeatEvent(int& beat);
    void onTapEvent(string& name);
    void onPlayEvent(string& name);
    void onStopEvent(string& name);
    void onChangeEvent(string& name);
    
private:

	ofParameter<string> midiInterface;

    int currentSequence;
    ofxOscReceiver receiver;
    ofxMidiOut midiOut;
    
    ofParameter<int> port;
    midiMessage tMessage;
    
    ofxSequencer sequencer;
    
    ofParameter<int> p1;
    ofParameter<int> p2;
    ofParameter<int> p3;
    ofParameter<int> p4;
    ofParameter<int> p5;
    ofParameter<int> p6;
    ofParameter<int> p7;
    ofParameter<int> p8;
    ofParameter<int> p9;
    ofParameter<int> p10;
    
    vector<Button*> buttons;
    
    vector<long> timestamps;
    ofParameter<int> bpm;
    vector<int> indexMidi;
    
    ofxOscSender sender;

	bool isPlaying;
    
    vector< vector<ofxSequencerRowBase*>> sequences;
};
