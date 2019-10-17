#pragma once

#include "ofxMidi.h"
#include "RadarAttributes.h"
#include "ofxAbletonLive.h"

class MidiCtrlSettings {
public:
	shared_ptr<RadarAttributes> radarAttributes;
	ofJson jsonSettings;
};

class MidiCtrl: public ofxMidiListener
{
public:
	MidiCtrl();
	~MidiCtrl();

	void setup(MidiCtrlSettings settings);
	void update();

	void newMidiMessage(ofxMidiMessage& eventArgs);
	void onBeat(int& beat);
	void sendMidi(int channel, int note, int value);

private:
	shared_ptr<RadarAttributes> radar;
	ofxMidiIn midiIn;
	ofxMidiOut midiOut;
	ofxAbletonLive live;

	
};

