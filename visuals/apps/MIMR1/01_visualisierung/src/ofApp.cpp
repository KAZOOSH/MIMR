#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowShape(1920,1200);
    //ofSetWindowPosition(1280, 0);
    //ofSetFullscreen(false);
    ofLogToConsole();
    
    p.add(params.v_kuehler1.set("kuehler 1",false));
    p.add(params.v_kuehler2.set("kuehler 2",false));
    p.add(params.v_kuehler3.set("kuehler 3",false));
    p.add(params.v_kuehler4.set("kuehler 4",false));
    p.add(params.v_kuehler5.set("kuehler 5",false));
    p.add(params.v_theremin.set("theremin",0,0,127));
    p.add(params.v_trichter_rot.set("trichter_rot",0,0,127));
    p.add(params.v_trichter_mic.set("trichter_mic",0,0,127));
    p.add(params.v_eieiei_spin.set("eieiei_spin",0,0,127));
    p.add(params.v_eieiei_membrane.set("eieiei_membrane",0,0,127));
    p.add(params.v_goldenBox.set("golden box",0,0,11));
    p.add(params.v_kurbel.set("kurbel",0,0,127));
    p.add(params.v_bassfahrer.set("bassfahrer",0,0,127));
    p.add(params.v_foehn.set("foehn",0,0,127));
    p.add(params.v_foehn_schalter1.set("foehn schalter 1",false));
    p.add(params.v_foehn_schalter2.set("foehn schalter 2",false));
    p.add(params.v_foehn_schalter3.set("foehn schalter 3",false));
    p.add(params.colorSet.set("colorSet", 0, 0, 3));
    
    params.bpm = 100;
    params.colorSet.addListener(this, &ofApp::onSceneChanged);
    
    isGuiVisible = false;
    
    
    //midiPort.set("midiPort","nanoKONTROL2 SLIDER/KNOB");
    initMidi();
    
    gui.setup("panel");
    gui.add(p);
    sync.setup((ofParameterGroup&)gui.getParameter(), 6667, "localhost", 6666);
    
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    layer.push_back(new Universum());
    layer.push_back(new Kuehler());
    
    layer.push_back(new GoldenBox());
    layer.push_back(new Radar());
    layer.push_back(new Foehn());
    layer.push_back(new Kurbel());
    
    for(auto& l:layer){
        l->setup(&params);
    }
    
    ofEnableAntiAliasing();
    
    rotationFbo = 0;
    lastMessageSend = 0;

	isGuiVisible = true;
}


//--------------------------------------------------------------
void ofApp::update() {
    sync.update();
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    float bpmMultiplier = pow(params.bpm / 100,2.2); //100 base rate
    
    params.lastTime = params.time;
    
    params.time += ofGetLastFrameTime()*20*bpmMultiplier;
    
    
    params.lastAngle = params.angle;
    params.angle += ofGetLastFrameTime()*20*bpmMultiplier;
    int tAngle = params.angle/360;
    params.angle = params.angle - tAngle*360;
    
    if(midiMessageQueue.size()>0 && ofGetElapsedTimeMillis() - lastMessageSend > 50){
        midiOut << StartMidi() << midiMessageQueue[0].type << midiMessageQueue[0].channel << midiMessageQueue[0].message << FinishMidi();
        lastMessageSend = ofGetElapsedTimeMillis();
        midiMessageQueue.pop_front();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    for (auto& l:layer) {
        l->draw();
    }
    fbo.end();
    
    float sizeFbo = Layer::map(params.v_bassfahrer,0,127,1024,512,1.0);
    rotationFbo += ofMap(params.v_bassfahrer,0,127,0,params.time - params.lastTime);
    
    ofPushMatrix();
    ofTranslate(900, 540);
    //ofTranslate(512,512);
    ofPushMatrix();
    ofScale(-1,1);
    ofRotateDeg(rotationFbo);
    ofPushMatrix();
    
    fbo.draw(-sizeFbo*0.5,-sizeFbo*0.5,sizeFbo,sizeFbo);
    ofPopMatrix();
    ofPopMatrix();
    ofPopMatrix();
    
    if (isGuiVisible)
    {
        gui.draw();
    }
    
}

void ofApp::initMidi(){
    // print input ports to console
    midiIn.listInPorts(); // via instance
    //ofxMidiIn::listPorts(); // via static as well
	
    
    //hack for finding the correct midi port automatically
    midiOut.listOutPorts();
    if (!midiOut.openPort("mimr_in 1")) {
        if (!midiOut.openPort("mimr_in 0")) {
            midiOut.openPort("mimr_in 2");
        }
    }
    
    // open port by number (you may need to change this)
    if (!midiIn.openPort("mimr_in 0")) {
        if (!midiIn.openPort("mimr_in 0")) {
            midiIn.openPort("mimr_in 2");
        }
    }
    //midiIn.openPort("IAC Pure Data In");	// by name
    //midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port
    
    // don't ignore sysex, timing, & active sense messages,
    // these are ignored by default
    midiIn.ignoreTypes(false, false, false);
    
    // add ofApp as a listener
    midiIn.addListener(this);
    
    // print received messages to the console
    midiIn.setVerbose(true);
}

void ofApp::exit(){
    // clean up
    midiIn.closePort();
    midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
	cout << ofxMidiMessage::getStatusString(msg.status) << endl;
    switch (msg.channel) {
        case 1:
            switch (msg.control) {
                case 0:
                    if(msg.value == 0) params.v_kurbel = 0;
                    break;
                case 1:
                    params.v_kurbel = msg.value;
                    break;
                    //beat
                case 3:
                    if(msg.value > 0){
                        float tTemp = ofGetElapsedTimef();
                        params.bpm = params.bpm*0.5 + 0.5*( 60.0/(tTemp - params.lastTick));
                        params.lastTick = tTemp;
                        //cout << bpm <<endl;
                    }
                    break;
                    //scene
                case 119:
                    if(msg.value< 5)
                        params.colorSet = msg.value;
                    break;
            }
            break;
        case 2:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) {
                        params.v_kuehler1 = 0;
                        params.v_kuehler2 = 0;
                        params.v_kuehler3 = 0;
                        params.v_kuehler4 = 0;
                        params.v_kuehler5 = 0;
                    }
                    break;
                case 1:
                    params.v_kuehler1 = msg.value;
                    break;
                case 2:
                    params.v_kuehler2 = msg.value;
                    break;
                case 3:
                    params.v_kuehler3 = msg.value;
                    break;
                case 4:
                    params.v_kuehler4 = msg.value;
                    break;
                case 5:
                    params.v_kuehler5 = msg.value;
                    break;
                default:
                    break;
            }
            break;
        case 3:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) params.v_theremin = 0;
                    break;
                case 1:
                    params.v_theremin = msg.value;
                    break;
            }
            break;
        case 4:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) {
                        params.v_trichter_rot = 0;
                        params.v_trichter_mic = 0;
                    }
                    break;
                case 1:
                    params.v_trichter_rot = msg.value;
                    break;
                case 2:
                    params.v_trichter_mic = msg.value;
                    break;
            }
            break;
        case 5:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) {
                        params.v_eieiei_membrane = 0;
                        params.v_eieiei_spin = 0;
                    }
                    break;
                case 1:
                    params.v_eieiei_membrane = msg.value;
                    break;
                case 2:
                    params.v_eieiei_spin = msg.value;
                    break;
            }
            break;
        case 6:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) params.v_goldenBox = 0;
                    break;
                case 1:
                    params.v_goldenBox = msg.value/12;
                    if (msg.value == 127)
                    {
                        params.v_goldenBox = 0;
                    }
                    break;
            }
            for (int i = 2; i < 12; i++)
            {
                if (msg.control == i && msg.value == 127)
                    params.v_goldenBox = i - 2;
            }
            break;
        case 7:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) params.v_bassfahrer = 0;
                    break;
                case 1:
                    params.v_bassfahrer = msg.value;
                    break;
            }
            break;
        case 8:
            switch (msg.control) {
                case 0:
                    if (msg.value == 0) {
                        params.v_foehn_schalter1 = 0;
                        params.v_foehn_schalter2 = 0;
                        params.v_foehn_schalter3 = 0;
                        params.v_foehn = 0;
                    }
                    break;
                case 1:
                    params.v_foehn_schalter1 = msg.value;
                    break;
                case 2:
                    params.v_foehn_schalter2 = msg.value;
                    break;
                case 3:
                    params.v_foehn_schalter3 = msg.value;
                    break;
                case 4:
                    params.v_foehn = msg.value;
                    break;
            }
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == '0')
    {
        params.colorSet = 0;
    }
    if (key == '1')
    {
        params.colorSet = 1;
    }
    if (key == '2')
    {
        params.colorSet = 2;
    }
    if (key == '3')
    {
        params.colorSet = 3;
    }
    if (key == 'f') {
        ofToggleFullscreen();
    }
    if(key == 'r'){
        for (auto& l:layer) {
            l->reload();
        }
    }
    if (key == 'g') { 
		isGuiVisible != isGuiVisible; 
	}
    if(key == 's'){
        ofFbo fbo;
        fbo.allocate(1024, 1024);
        for (auto& l:layer) {
            fbo.begin();
            ofClear(0, 0, 0,0);
            l->draw();
            fbo.end();
            ofPixels p;
            fbo.readToPixels(p);
            ofImage img;
            img.setFromPixels(p);
            img.save("screenshots/" + ofGetTimestampString() + ".png");
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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

void ofApp::onSceneChanged(int& nScene){
    vector<uint8_t> bytes;
    
    int kurbelColor;
    uint8_t kuehlerHue;
    uint8_t kuehlerSat;
    int theremin;
    uint8_t trichter1;
    uint8_t trichter2;
    uint8_t eieieiColorSet;
    uint8_t goldenBoxMode;
    
    switch (nScene) {
        case 0:
            kurbelColor = 5;
            kuehlerHue = 10;
            kuehlerSat = 127;
            theremin = 5;
            trichter1 = 10;
            trichter2 = 80;
            eieieiColorSet = 0;
            goldenBoxMode = 127;
            
            break;
        case 1:
            kurbelColor = 3;
            kuehlerHue = 40;
            kuehlerSat = 255;
            theremin = 3;
            trichter1 = 20;
            trichter2 = 50;
            eieieiColorSet = 1;
            goldenBoxMode = 0;
            break;
        case 2:
            kurbelColor = 2;
            kuehlerHue = 0;
            kuehlerSat = 255;
            theremin = 2;
            trichter1 = 0;
            trichter2 = 20;
            eieieiColorSet = 2;
            goldenBoxMode = 0;
            break;
        case 3:
            kurbelColor = 7;
            kuehlerHue = 65;
            kuehlerSat = 255;
            theremin = 7;
            trichter1 = 60;
            trichter2 = 80;
            eieieiColorSet = 3;
            goldenBoxMode = 127;
            break;
            
        default:
            break;
    }
    
    midiMessageQueue.push_back(MidiMessage(0xB0,15,(uint8_t)(kurbelColor*127/8)));
    midiMessageQueue.push_back(MidiMessage(0xB0,25,kuehlerHue));
    midiMessageQueue.push_back(MidiMessage(0xB0,26,kuehlerSat));
    midiMessageQueue.push_back(MidiMessage(0xB0,35,(uint8_t)(theremin * 127 / 8)));
    midiMessageQueue.push_back(MidiMessage(0xB0,45,trichter1));
    midiMessageQueue.push_back(MidiMessage(0xB0,46,trichter2));
    midiMessageQueue.push_back(MidiMessage(0xB0,55,eieieiColorSet));
    midiMessageQueue.push_back(MidiMessage(0xB0,65,goldenBoxMode));
}
