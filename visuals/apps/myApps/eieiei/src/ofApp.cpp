#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	settings = ofLoadJson("settings.json");
    
    shader.load("shadersGL2/shader");
	ofLogToConsole();
    
    
    img.allocate(800,600,OF_IMAGE_COLOR_ALPHA);
    fbo.allocate(800,600);
	ofSetWindowShape(800, 600);

	ofSetWindowPosition(settings["position"], 0);
	//ofSetFullscreen(settings["fullScreen"]);

    
    
   /* std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    
    ofLogNotice("ofApp::setup") << "Connected Devices: ";
    
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLogNotice("ofApp::setup") << "\t" << devicesInfo[i];
    }*/
    
    gui.setup();
    
    gui.add(time.set("time",0,0,150));
    gui.setWidthElements(700);
    gui.setPosition(500,20);
    
    n=0;

	
	osc.setup(settings["port"].get<int>());
	/*midiIn.listInPorts();

	midiIn.openPort(settings["port"].get<string>());
	midiIn.ignoreTypes(false, false, false);
	midiIn.addListener(this);*/
}


//--------------------------------------------------------------
void ofApp::update() {
	
    
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
	time += ofGetLastFrameTime();// *ofMap(depth, 0, 127, 1.0, 1.9);

	while (osc.hasWaitingMessages()) {

		// get the next message
		ofxOscMessage m;
		osc.getNextMessage(m);

		if (m.getAddress() == "/mimr/instrument") {
			int channel = m.getArgAsInt(0);
			int control = m.getArgAsInt(1);
			int value = m.getArgAsInt(2);

			if (channel == settings["channel"]) {
				if (control == 0) {
					//radar->instruments[channel].isActive = value == 127 ? true : false;
				}
				else if (control == settings["control"]){
					zoom = ofMap(value, 0, 127, 0, PI / 2 - 0.5);
				}
			}
			
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	
    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    fbo.begin();
    ofClear(0,0,0,0);
    
    // start our shader, in our OpenGL3 shader this will automagically set
    // up a lot of matrices that we want for figuring out the texture matrix
    // and the modelView matrix
    shader.begin();
    shader.setUniform1f("iTime", time);
    shader.setUniform1f("depth", zoom);
    
    
    ofPushMatrix();
    
    img.draw(0,0);
    
    ofPopMatrix();
    
    shader.end();
    
    fbo.end();
    
    fbo.draw(0,0,800,600);
    
   // gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'r') {
        shader.load("shadersGL2/shader");
    }
    
    if (key == 's') {
        ofPixels p;
        fbo.readToPixels(p);
        ofImage im;
        im.setFromPixels(p);
        //im.save("img/pic_" + ofGetTimestampString() + ".png");
        im.save("img/pic_" + ofToString(n) + ".png");
        n++;
    }

	if (key == 'f') ofToggleFullscreen();
    
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

void ofApp::newMidiMessage(ofxMidiMessage & eventArgs)
{
	if (eventArgs.channel == settings["channel"] && eventArgs.control == settings["control"]) {
		zoom = ofMap(eventArgs.value, 0, 127, 0, PI/2-0.5);
	}
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
