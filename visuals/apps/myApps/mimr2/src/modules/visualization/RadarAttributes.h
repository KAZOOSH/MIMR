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
	string effect = "lineDistance";

	// current values
	ofParameter<bool> isActive;
	vector<ofParameter<int>> values;

	// rendering params
	ofFbo fboWave;
	ofFbo fboTex;
	ofFbo fboShaper;
	ofFbo fboAbberation;
	float posLastRadarWave = 0;
};

class ChaosObject : public Instrument{
public:
	void setup(int channel , int note, ofJson renderSettings);

	// attributes
	ofVec2f positionXY;
	int note;
	bool hasSend = false;
	long lastActivated = 0;
	
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
	float getBeatLength();

protected:
	float getAngleFromBeat(float beat);

private:
	int nBars = 4;
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
	map<string, vector<shared_ptr<Instrument>>> instrumentByEffect;
	map<int, ChaosObject> chaosObjects;

};