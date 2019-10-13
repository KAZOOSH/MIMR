#include "MidiCtrl.h"



MidiCtrl::MidiCtrl()
{
}


MidiCtrl::~MidiCtrl()
{
}

void MidiCtrl::setup(MidiCtrlSettings settings)
{
	radar = settings.radarAttributes;

	// print input ports to console
	midiIn.listInPorts();

	midiIn.openPort(settings.jsonSettings["port"].get<string>());
	midiIn.ignoreTypes(false, false, false);
	midiIn.addListener(this);
	//midiIn.setVerbose(true);

	live.setup(settings.jsonSettings["abletonIp"].get<string>());
	live.getBeat().addListener(this, &MidiCtrl::onBeat);
}

void MidiCtrl::update()
{
	live.update();
	radar->beat.update();
}

void MidiCtrl::newMidiMessage(ofxMidiMessage & eventArgs)
{	
	if (eventArgs.control == 0) {
		radar->instruments[eventArgs.channel].isActive = eventArgs.value == 127 ? true : false;
	}
	else {
		radar->instruments[eventArgs.channel].values[eventArgs.control-1] = eventArgs.value;
	}
}

void MidiCtrl::onBeat(int & beat)
{
	radar->beat.newBeat(beat);
}
