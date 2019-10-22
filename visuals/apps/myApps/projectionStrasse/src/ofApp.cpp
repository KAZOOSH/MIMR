#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();

	json = ofLoadJson("settings.json");

	rendering.allocate(1920,1080);
	rendering.begin();
	ofClear(0, 0);
	rendering.end();

	circle.allocate(1080, 1080);
	circle.begin();
	ofClear(0, 0);
	circle.end();

	int x = (ofGetWidth() - rendering.getWidth()) * 0.5;       // center on screen.
	int y = (ofGetHeight() - rendering.getHeight()) * 0.5;     // center on screen.
	int w = rendering.getWidth();
	int h = rendering.getHeight();


	warper.setSourceRect(ofRectangle(0, 0, w, h));              // this is the source rectangle which is the size of the image and located at ( 0, 0 )
	warper.setTopLeftCornerPosition(ofPoint(x, y));             // this is position of the quad warp corners, centering the image on the screen.
	warper.setTopRightCornerPosition(ofPoint(x + w, y));        // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomLeftCornerPosition(ofPoint(x, y + h));      // this is position of the quad warp corners, centering the image on the screen.
	warper.setBottomRightCornerPosition(ofPoint(x + w, y + h)); // this is position of the quad warp corners, centering the image on the screen.
	warper.setup();
	warper.load(); // reload last saved changes.

	warper.load();

	loadShader();

	ofSetCircleResolution(400);

	font.load("CALADEA-BOLD.TTF",json["fontSize"]);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	ofSetColor(255);
	float angle = 45;

	int w = 1080;

	

	rendering.begin();
	ofClear(0, 0);
	waves.begin();
	waves.setUniform1i("width", w);
	waves.setUniform1i("height", w);
	waves.setUniform2f("center", ofVec2f(w*0.5 + cos(angle)*w*0.5, w*0.5 + sin(angle)*w*0.5));
	waves.setUniform1f("time", ofGetElapsedTimef());
	ofDrawCircle(w*0.5, w*.5, w*0.5);
	waves.end();
	ofPushMatrix();
	ofTranslate(json["x"], json["y"]);
	ofPushMatrix();
	ofRotate(-90);

	ofSetColor(255, 179, 0);
	font.drawString("musicismyradar", 0,0);
	ofPopMatrix();
	ofPopMatrix();
	rendering.end();

	

	//rendering.draw((ofGetWidth()-rendering.getWidth())*0.5, 0);

	//======================== get our quad warp matrix.

	ofMatrix4x4 mat = warper.getMatrix();

	//======================== use the matrix to transform our fbo.


	ofPushMatrix();
	ofMultMatrix(mat);
	rendering.draw(0, 0);
	if (warper.isShowing()) {

	}
	ofPopMatrix();

	//======================== draw quad warp ui.

	ofSetColor(ofColor::magenta);
	warper.drawQuadOutline();

	ofSetColor(ofColor::yellow);
	warper.drawCorners();

	ofSetColor(ofColor::magenta);
	warper.drawHighlightedCorner();

	ofSetColor(ofColor::red);
	warper.drawSelectedCorner();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'w') {
		loadShader();
	}

	if (key == 'd') {
		warper.toggleShow();
	}

	if (key == 's') {
		warper.save();
	}
	if (key == 'f') {
		ofToggleFullscreen();
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

void ofApp::loadShader()
{
	waves.load("waves");
}

void ofApp::drawRing(ofVec2f center, float radius, float width, ofColor color)
{
	ofPath ring;
	ring.setColor(color);
	ring.setCircleResolution(400);
	ring.circle(center, radius);
	ring.circle(center, radius - width);
	ring.close();
	ring.draw();
}



//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
