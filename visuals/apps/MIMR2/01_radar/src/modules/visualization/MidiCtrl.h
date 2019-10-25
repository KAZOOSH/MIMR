#pragma once

#include "ofxMidi.h"
#include "RadarAttributes.h"
#include "ofxAbletonLive.h"
#include "ofxOsc.h"

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
	void sendNote(int channel, int note, bool isOn);
protected:

	void updateOsc();

private:
	shared_ptr<RadarAttributes> radar;
	float diameter;
	ofxMidiIn midiIn;
	ofxMidiOut midiOut;
	ofxAbletonLive live; 

	ofxOscReceiver oscIn;
	ofxOscSender oscOut;

	
};

