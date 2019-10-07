#pragma once
#include "ofMain.h"
#include "GeneralPurposeFunctions.h"

class Instrument {
public:
	void setup(ofJson midiSettings,ofJson renderSettings);

	// attributes
	string name;
	int channel;
	int position;

	// current values
	ofParameter<bool> isActive;
	vector<ofParameter<int>> values;

	// rendering params
	ofFbo fboWave;
	ofFbo fboTex;
	ofFbo fboShaper;
	ofFbo fboAbberation;
};

class BeatValue {
public:
	float beat = 0;
	float angle = 0;
	long timestamp = 0;

	BeatValue operator-(BeatValue b) {
		BeatValue ret;
		ret.beat = beat - b.beat;
		ret.angle = angle - b.angle;
		ret.timestamp = timestamp - b.timestamp;
		return ret;
	}
};

class Beat {
public:
	void update();
	void newBeat(int value);
	BeatValue getCurrentBeat();
	BeatValue getLastBeat();
	void setNBars(int n);

protected:
	float getAngleFromBeat(float beat);

private:
	int nBars = 1;
	int nBeats = 4;
	BeatValue diffBeat;
	BeatValue currentBeat;
	BeatValue lastBeat;
};

class RadarAttributes {
public:
	RadarAttributes();
	~RadarAttributes();
	void setup(ofJson instruments, ofJson renderSettings);

	ofParameterGroup params;
	Beat beat;

	ofParameter<int> colorSet;
	//  values for objects
	map<int,Instrument> instruments;


};