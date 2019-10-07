#include "RadarAttributes.h"



RadarAttributes::RadarAttributes()
{

}


RadarAttributes::~RadarAttributes()
{
}

void RadarAttributes::setup(ofJson jinstruments, ofJson renderSettings)
{
	for (auto& entry : jinstruments) {
		instruments.insert(pair<int,Instrument>(entry["channel"],Instrument()));
		instruments[entry["channel"]].setup(entry,renderSettings);
		
		params.add(instruments[entry["channel"]].isActive);
		for (auto& val : instruments[entry["channel"]].values) {
			params.add(val);
		}
	}
}

void Instrument::setup(ofJson settings, ofJson renderSettings)
{
	name = settings["name"].get<string>();
	channel = settings["channel"];
	position = settings["position"];
	isActive.set(name + " isActive",false);
	for (int i = 0; i < settings["values"] - 1; ++i) {
		values.push_back(ofParameter<int>());
		values.back().set(name + " value" + ofToString(i), 0, 0, 127);
	}

	fboWave.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);
	fboTex.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);
	fboShaper.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);
	fboAbberation.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);

	fboTex.begin();
	ofClear(0, 255);
	fboTex.end();
}

void Beat::update()
{
	auto now = ofGetElapsedTimeMillis();

	// get the difference beat as reference 
	BeatValue diffBeat = currentBeat - lastBeat;
	long tDiff = now - currentBeat.timestamp;
	
	// get the current time marker
	float multiplicator = tDiff / diffBeat.timestamp;
	
	// calculate current beat 
	lastBeat = currentBeat;
	currentBeat.timestamp = now;
	currentBeat.beat += diffBeat.beat*multiplicator;
	currentBeat.angle = getAngleFromBeat(currentBeat.beat);
	
}

void Beat::newBeat(int value)
{
	lastBeat = currentBeat;

	currentBeat.timestamp = ofGetElapsedTimeMillis();
	currentBeat.beat = value;
	currentBeat.angle = getAngleFromBeat(value);
}

BeatValue Beat::getCurrentBeat()
{
	return currentBeat;
}

BeatValue Beat::getLastBeat()
{
	return lastBeat;
}

void Beat::setNBars(int n)
{
	nBars = n;
}

float Beat::getAngleFromBeat(float beat)
{
	return fmod(beat,nBars*nBeats);
}


