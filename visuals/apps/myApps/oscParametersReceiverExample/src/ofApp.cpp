#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	p.add(v_kuehler1.set("kuehler 1", false));
	p.add(v_kuehler2.set("kuehler 2", false));
	p.add(v_kuehler3.set("kuehler 3", false));
	p.add(v_kuehler4.set("kuehler 4", false));
	p.add(v_kuehler5.set("kuehler 5", false));
	p.add(v_theremin.set("theremin", 0, 0, 127));
	p.add(v_trichter_rot.set("trichter_rot", 0, 0, 127));
	p.add(v_trichter_mic.set("trichter_mic", 0, 0, 127));
	p.add(v_eieiei_spin.set("eieiei_spin", 0, 0, 127));
	p.add(v_eieiei_membrane.set("eieiei_membrane", 0, 0, 127));
	p.add(v_goldenBox.set("golden box", 0, 0, 11));
	p.add(v_kurbel.set("kurbel", 0, 0, 127));
	p.add(v_bassfahrer.set("bassfahrer", 0, 0, 127));
	p.add(v_foehn.set("foehn", 0, 0, 127));
	p.add(v_foehn_schalter1.set("foehn schalter 1", false));
	p.add(v_foehn_schalter2.set("foehn schalter 2", false));
	p.add(v_foehn_schalter3.set("foehn schalter 3", false));
	p.add(colorSet.set("colorSet", 0, 0, 3)); 
	gui.setup(p);
	// by now needs to pass the gui parameter groups since the panel internally creates it's own group
	sync.setup((ofParameterGroup&)gui.getParameter(),6666,"localhost",6667);
	ofSetVerticalSync(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	sync.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	gui.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
