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
		auto in = pair<int, Instrument>(entry["channel"], Instrument());
		in.second.setup(entry, renderSettings);
		instruments.insert(in);
		//instruments[entry["channel"]].setup(entry,renderSettings);
		//instrumentByEffect.insert(pair<string, shared_ptr<Instrument>>(entry["effect"].get<string>(), shared_ptr<Instrument>(&instruments[entry["channel"]])));
		if (instrumentByEffect.find(in.second.effect) != instrumentByEffect.end()) {
			instrumentByEffect[in.second.effect].push_back(shared_ptr<Instrument>(&instruments[entry["channel"]]));
		}else {
			vector<shared_ptr<Instrument>> v;
			v.push_back(shared_ptr<Instrument>(&instruments[entry["channel"]]));
			instrumentByEffect.insert(pair<string, vector<shared_ptr<Instrument>>>(in.second.effect, v));
		}


		params.add(in.second.isActive);
		for (auto& val : in.second.values) {
			params.add(val);
		}

		/*params.add(instruments[entry["channel"]].isActive);
		for (auto& val : instruments[entry["channel"]].values) {
			params.add(val);
		}*/
	}
}

void Instrument::setup(ofJson settings, ofJson renderSettings)
{
	name = settings["name"].get<string>();
	channel = settings["channel"];
	position = settings["position"];
	effect = settings["effect"];
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



	long tDiff = now - lastBeat.timestamp;

	// get the current time marker
	currentBeat.beat = ofMap(now, lastBeat.timestamp, lastBeat.timestamp + diffBeat.timestamp, lastBeat.beat, lastBeat.beat + 1);

	// calculate current beat 
	//lastBeat = currentBeat;
	currentBeat.timestamp = now;
	//currentBeat.beat += diffBeat.beat*multiplicator;
	currentBeat.angle = getAngleFromBeat(currentBeat.beat);
}

void Beat::newBeat(int value)
{
	//diffBeat = currentBeat;
	BeatValue bt;
	bt.timestamp = ofGetElapsedTimeMillis();
	bt.beat = value;
	bt.angle = getAngleFromBeat(value);

	diffBeat = bt - lastBeat;
	currentBeat = bt;
	lastBeat = currentBeat;
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

float Beat::getBeatLength()
{
	return diffBeat.timestamp;
}

float Beat::getAngleFromBeat(float beat)
{
	return fmod(beat,nBars*nBeats)*2*PI / (nBars*nBeats);
}


