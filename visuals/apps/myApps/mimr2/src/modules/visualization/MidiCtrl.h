#pragma once

#include "ofxMidi.h"

class MidiCtrl: public ofxMidiListener
{
public:
	MidiCtrl();
	~MidiCtrl();

	void newMidiMessage(ofxMidiMessage& eventArgs);

	ofxMidiIn midiIn;
	
};

