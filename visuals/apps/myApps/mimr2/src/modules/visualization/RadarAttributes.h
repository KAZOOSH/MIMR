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

class Beat {
public:
	void newBeat(int value);
	float getCurrentValue();
	float getCurrentAngle();
	long getLastBeat();
	void setNBars(int n);

private:
	int nBars = 1;
	long lastTimestamp = 0;
	long dsTimestamp = 0;
	float dsTime = 0;
	float lastTime = 0;
	float lastAngle = 0;
	float currentAngle = 0;
};

class RadarAttributes {
public:
	RadarAttributes();
	~RadarAttributes();
	void setup(ofJson instruments, ofJson renderSettings);

	ofParameterGroup params;

	ofParameter<int> colorSet;
	//  values for objects
	map<int,Instrument> instruments;


};