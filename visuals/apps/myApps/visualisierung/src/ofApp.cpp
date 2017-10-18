#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowShape(1024,1024);
    
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
    
    midiPort.set("midiPort","Netzwerk Session 1");
    //midiPort.set("midiPort","nanoKONTROL2 SLIDER/KNOB");
    initMidi();
    
    gui.setup("panel");
    gui.add(p);
    
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    layer.push_back(new Universum());
    layer.push_back(new Kuehler());
    
    layer.push_back(new GoldenBox());
    layer.push_back(new Radar());
    layer.push_back(new Foehn());
    //layer.push_back(new Kurbel());
    
    for(auto& l:layer){
        l->setup(&params);
    }
    
    ofEnableAntiAliasing();
    
    rotationFbo = 0;
}


//--------------------------------------------------------------
void ofApp::update() {
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    params.lastAngle = params.angle;
    params.angle = ofGetElapsedTimef()*20;
    if (params.angle >360) {
        params.angle -= 360;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(80);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    for (auto& l:layer) {
        l->draw();
    }
    fbo.end();
    
    float sizeFbo = Layer::map(params.v_bassfahrer,0,127,1024,512,1.0);
    rotationFbo += ofMap(params.v_bassfahrer,0,127,0,params.angle - params.lastAngle);
    
    ofPushMatrix();
    ofTranslate(512,512);
    ofPushMatrix();
    ofScale(-1,1);
    ofRotateDeg(rotationFbo);
    ofPushMatrix();
    
    fbo.draw(-sizeFbo*0.5,-sizeFbo*0.5,sizeFbo,sizeFbo);
    ofPopMatrix();
    ofPopMatrix();
    ofPopMatrix();
    gui.draw();
}

void ofApp::initMidi(){
    // print input ports to console
    midiIn.listPorts(); // via instance
    //ofxMidiIn::listPorts(); // via static as well
    
    // open port by number (you may need to change this)
    midiIn.openPort(midiPort.get());
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
    switch (msg.channel) {
        case 1:
            params.v_kurbel = msg.value;
            break;
        case 2:
            switch (msg.control) {
                case 0:
                    params.v_kuehler1 = msg.value;
                    break;
                case 1:
                    params.v_kuehler2 = msg.value;
                    break;
                case 2:
                    params.v_kuehler3 = msg.value;
                    break;
                case 3:
                    params.v_kuehler4 = msg.value;
                    break;
                case 4:
                    params.v_kuehler5 = msg.value;
                    break;
                default:
                    break;
            }
            break;
        case 3:
            params.v_theremin = msg.value;
            break;
        case 4:
            switch (msg.control) {
                case 0:
                    params.v_trichter_rot = msg.value;
                    break;
                case 1:
                    params.v_trichter_mic = msg.value;
                    break;
            }
            break;
        case 5:
            switch (msg.control) {
                case 0:
                    params.v_eieiei_membrane = msg.value;
                    break;
                case 1:
                    params.v_eieiei_spin = msg.value;
                    break;
            }
            break;
        case 6:
            params.v_goldenBox = msg.value;
            break;
        case 7:
            params.v_bassfahrer = msg.value;
            break;
        case 8:
            switch (msg.control) {
                case 0:
                    params.v_foehn_schalter1 = msg.value;
                    break;
                case 1:
                    params.v_foehn_schalter2 = msg.value;
                    break;
                case 2:
                    params.v_foehn_schalter3 = msg.value;
                    break;
                case 3:
                    params.v_foehn = msg.value;
                    break;
            }
            break;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'r'){
        for (auto& l:layer) {
            l->reload();
        }
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
