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
	//midiIn.listInPorts();

	//midiIn.openPort(settings.jsonSettings["port"].get<string>());
	//midiIn.ignoreTypes(false, false, false);
	//midiIn.addListener(this);
	////midiIn.setVerbose(true);

	//midiOut.listOutPorts();
	//midiOut.openPort(settings.jsonSettings["portOut"].get<string>());

	//live.setup(settings.jsonSettings["abletonIp"].get<string>());
	//live.getBeat().addListener(this, &MidiCtrl::onBeat);

	oscIn.setup(9002);
	oscOut.setup(settings.jsonSettings["midi"]["oscOut"].get<string>(), 9002);
	diameter = settings.jsonSettings["rendering"]["radarSize"][0];
}

void MidiCtrl::update()
{
	updateOsc();
	//live.update();
	radar->beat.update();
}

void MidiCtrl::newMidiMessage(ofxMidiMessage & eventArgs)
{	
	/*if (eventArgs.control == 0) {
		radar->instruments[eventArgs.channel].isActive = eventArgs.value == 127 ? true : false;
	}
	else {
		radar->instruments[eventArgs.channel].values[eventArgs.control-1] = eventArgs.value;
	}*/
}

void MidiCtrl::onBeat(int & beat)
{
	radar->beat.newBeat(beat);
}

void MidiCtrl::sendMidi(int channel, int note, int value)
{
	//midiOut.sendControlChange(channel, note, value);

	ofxOscMessage m;
	m.setAddress("/mimr/chaos");
	m.addStringArg("chaos");
	m.addIntArg(176 + channel-1);
	m.addIntArg(note);
	m.addIntArg(value);
	oscOut.sendMessage(m, false);
}

void MidiCtrl::sendNote(int channel, int note, bool isOn)
{
	/*if (isOn) {
		midiOut.sendNoteOn(channel, note);
	}
	else {
		midiOut.sendNoteOff(channel, note);
	}*/
}

void MidiCtrl::updateOsc()
{
	// check for waiting messages
	while (oscIn.hasWaitingMessages()) {

		// get the next message
		ofxOscMessage m;
		oscIn.getNextMessage(m);

		if (m.getAddress() == "/mimr/instrument") {
			int channel = m.getArgAsInt(0);
			int control = m.getArgAsInt(1);
			int value = m.getArgAsInt(2);

			if (radar->instruments.find(channel) != radar->instruments.end()) {
				if (control == 0) {
					radar->instruments[channel].isActive = value == 127 ? true : false;
				}
				else {
					radar->instruments[channel].values[control - 1] = value;
				}
			}
			
		}
		else if (m.getAddress() == "/mimr/beat") {
			radar->beat.newBeat(m.getArgAsInt(0));
		}
		else if (m.getAddress() == "/chaos") {
			int id = m.getArgAsInt32(0);
			bool isActive = m.getArgAsBool(1);
			float x = m.getArgAsFloat(2);
			float y = m.getArgAsFloat(3);

			for (auto& chaos : radar->chaosObjects) {
				if (chaos.second.note == id) {
					chaos.second.positionXY = ofVec2f(x*diameter, y*diameter);
					if (isActive && chaos.second.positionXY.distance(ofVec2f(diameter, diameter)*.5) < diameter*0.5) {
						chaos.second.isActive = true;
						//cout << chaos.second.note << "  active " << chaos.second.isActive << "  pos" << x << "," << y << "     "<< chaos.second.positionXY<<endl;
					}
					else {
						chaos.second.isActive = false;
					}
				}
			}
		}
	}
	
}
