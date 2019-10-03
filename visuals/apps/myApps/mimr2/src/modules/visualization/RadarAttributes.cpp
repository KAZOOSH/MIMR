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

	fboFinal.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);
	fboTex.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);
	fboShaper.allocate(renderSettings["radarSize"][0], renderSettings["radarSize"][1]);

	fboTex.begin();
	ofClear(0, 255);
	fboTex.end();
}
