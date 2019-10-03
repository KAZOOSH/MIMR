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
	ofFbo fboFinal;
	ofFbo fboTex;
	ofFbo fboShaper;
};

class RadarAttributes {
public:
	RadarAttributes();
	~RadarAttributes();
	void setup(ofJson instruments, ofJson renderSettings);

	// values for radar
	float lastAngle = 0;
	float currentAngle = 0;

	long lastTimestamp = 0;
	long dsTimestamp = 0;
	float dsTime = 0;
	float lastTime = 0;

	ofParameterGroup params;

	ofParameter<int> colorSet;
	//  values for objects
	map<int,Instrument> instruments;


};