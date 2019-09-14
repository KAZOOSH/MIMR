#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	midiInterface.set("midiInterface", "midi");
    
	ofXml xml;
	//ofSerialize(xml, midiInterface);
	//xml.save("settings.xml");
	xml.load("settings.xml");
	ofDeserialize(xml, midiInterface);
	
	
	
	//port.set("port",12345);
   // receiver.setup(port);
    midiOut.listPorts();
   // midiOut.openPort(5);
	midiOut.openPort(midiInterface.get());
    ofToggleFullscreen();
    
    ofBackground(70);
    
    //ofSetOrientation(OF_ORIENTATION_90_RIGHT);
    // ofSetOrientation(OF_ORIENTATION_90_RIGHT);
	isPlaying = true;
    
    bpm = 120;
    sequencer.setup(16, bpm, 16);
    
    p1.set("bass drum", 0, 0, 127);
    p2.set("floor tom", 0, 0, 127);
    p3.set("low mid tom", 0, 0, 127);
    p4.set("high tom", 0, 0, 127);
    p5.set("low wood block", 0, 0, 127);
    p6.set("high wood block", 0, 0, 127);
    p7.set("claves", 0, 0, 127);
    p8.set("closed hihat", 0, 0, 127);
    p9.set("crash/cymbal", 0, 0, 127);
    
    indexMidi.push_back(36); // bass drum
    indexMidi.push_back(41); // floor tom
    indexMidi.push_back(47); // low mid tom
    indexMidi.push_back(50); // high tom
    indexMidi.push_back(77); // low wood block
    indexMidi.push_back(76); // high wood block
    indexMidi.push_back(75); // claves
    // indexMidi.push_back(70); // maracas
    indexMidi.push_back(42); // closed hihat
    indexMidi.push_back(49); // crash/cymbal
    
    
    
    sequencer.addRow(&p1);
    sequencer.addRow(&p2);
    sequencer.addRow(&p3);
    sequencer.addRow(&p4);
    sequencer.addRow(&p5);
    sequencer.addRow(&p6);
    sequencer.addRow(&p7);
    sequencer.addRow(&p8);
    sequencer.addRow(&p9);
    
    //sequencer.randomize();
    sequencer.setSmooth(true);
    
    sequencer.start();
    sequencer.setMouseActive(false);
    
    sequencer.setPosition(0, 150, ofGetWidth(), ofGetHeight()-150);
    
    //sender.setup( HOST, PORT );
    
    ofAddListener(sequencer.beatEvent, this, &ofApp::onBeatEvent);
    
    buttons.push_back(new Button("tap",100,10,100,100));
    ofAddListener(buttons.back()->buttonPressedEvent, this, &ofApp::onTapEvent);
    
    buttons.push_back(new Button("play",250,10,100,100));
    ofAddListener(buttons.back()->buttonPressedEvent, this, &ofApp::onPlayEvent);
    
    buttons.push_back(new Button("stop",360,10,100,100));
    ofAddListener(buttons.back()->buttonPressedEvent, this, &ofApp::onStopEvent);
    
    for (int i=0; i<7; ++i) {
        sequences.push_back(vector<ofxSequencerRowBase*>());
        
        sequences.back().push_back(new ofxSequencerRow<int>(&p1, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p2, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p3, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p4, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p5, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p6, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p7, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p8, 16));
        sequences.back().push_back(new ofxSequencerRow<int>(&p9, 16));
        
        buttons.push_back(new Button(ofToString(i),500 + i*110,10,100,100));
        ofAddListener(buttons.back()->buttonPressedEvent, this, &ofApp::onChangeEvent);
    }
    currentSequence = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    // check for waiting messages
    /** while(receiver.hasWaitingMessages()){
     // get the next message
     ofxOscMessage m;
     receiver.getNextMessage(m);
     
     
     // check for mouse moved message
     if(m.getAddress() == "/note"){
     for (int i=0; i<10; ++i){
     // both the arguments are float's
     tMessage.intValue = m.getArgAsInt32(i);
     cout << (int)tMessage.message[1] << "  " << (int)tMessage.message[2] <<endl;
     if((int)tMessage.message[2] != 0){
     midiOut << NoteOn(1,tMessage.message[1],tMessage.message[2]);
     }
     }
     
     }
     }**/
    
    sequencer.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString("BPM: " + ofToString(bpm), 10, 20);
    sequencer.draw();
    
    
    for (int i=0; i<4; ++i) {
        ofSetLineWidth(4);
        ofDrawLine(i*ofGetWidth()/4, 145, i*ofGetWidth()/4, ofGetHeight());
    }
    
    
    
    for(auto& b:buttons){
        b->draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f'){
        ofToggleFullscreen();
    }
	if (key == '0')
	{
		string a = "0";
		onChangeEvent(a);
	}
	else if (key == '1')
	{
		string a = "1";
		onChangeEvent(a);
	}
	else if (key == '2')
	{
		string a = "2";
		onChangeEvent(a);
	}
	else if (key == '3')
	{
		string a = "3";
		onChangeEvent(a);
	}
	else if (key == '4')
	{
		string a = "4";
		onChangeEvent(a);
	}
	else if (key == '5')
	{
		string a = "5";
		onChangeEvent(a);
	}
	else if (key == '6')
	{
		string a = "6";
		onChangeEvent(a);
	}

	if (key == ' ') {
		if (isPlaying) {
			sequencer.stop(); 
			isPlaying = false;
		}
		else {
			sequencer.start();
			isPlaying = true;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofMouseEventArgs arg;
    arg.x = x;
    arg.y = y;
    sequencer.mouseDragged(arg);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    ofMouseEventArgs arg;
    arg.x = x;
    arg.y = y;
    sequencer.mousePressed(arg);
    
    for(auto& b:buttons){
        b->onPressed(arg.x, arg.y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ofMouseEventArgs arg;
    arg.x = x;
    arg.y = y;
    sequencer.mouseReleased(arg);
    
    for(auto& b:buttons){
        b->onReleased();
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

void ofApp::onBeatEvent(int& beat){
    
    ofxSequencerRow<int>* r = static_cast<ofxSequencerRow<int>*>(sequencer.getRows()[0]);
    //cout << r->getValue(0) << endl;
    
    ofxOscBundle b;
    ofxOscMessage m;
    m.setAddress( "/note");
    for (int i=0; i<sequencer.getRows().size(); ++i) {
        ofxSequencerRow<int>* r = static_cast<ofxSequencerRow<int>*>(sequencer.getRows()[i]);
        
        tMessage.message[0] = 12;
        tMessage.message[1] = indexMidi[i];
        tMessage.message[2] = r->getValue(beat);
        tMessage.message[3] = 0;
        
        if (tMessage.message[2] != 0){
            midiOut << NoteOn(1,tMessage.message[1],tMessage.message[2]);
        }
        //cout << i << " " << r->getValue(beat) <<endl;
        
        
        
        m.addIntArg(tMessage.intValue);
        
        //b.addMessage(m);
        
    }
   // sender.sendMessage(m);
}

void ofApp::onTapEvent(string& name){
    long now = ofGetElapsedTimeMillis();
    for (int i = timestamps.size()-1; i>=0; --i) {
        if (timestamps[i] + 5000 < now) {
            timestamps.erase(timestamps.begin() + i);
        }
    }
    
    timestamps.push_back(ofGetElapsedTimeMillis());
    
    if(timestamps.size()>1){
        vector<int> divs;
        for (int i=1; i<timestamps.size(); ++i) {
            divs.push_back(timestamps[i] - timestamps[i-1]);
        }
        
        
        int tTotal = 0;
        for (auto& t:divs) {
            tTotal += t;
        }
        tTotal /= divs.size();
        
        bpm = 60000/tTotal;
        sequencer.setBpm(bpm,16);
    }
    
}

void ofApp::onPlayEvent(string& name){
    
    sequencer.start();
}
void ofApp::onStopEvent(string& name){
    sequencer.stop();
    sequencer.reset();
}

void ofApp::onChangeEvent(string& name){
    int n = ofToInt(name);
    sequences[currentSequence] = sequencer.getRows();
    currentSequence = n;
    sequencer.setRows(sequences[currentSequence]);
}
