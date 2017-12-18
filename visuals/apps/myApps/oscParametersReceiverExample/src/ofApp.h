#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOscParameterSync.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		ofxPanel gui;
		ofParameterGroup p;
		ofxOscParameterSync sync;

		ofParameter<int> colorSet;
		//values for objects
		ofParameter<int> v_kurbel;
		ofParameter<bool> v_kuehler1;
		ofParameter<bool> v_kuehler2;
		ofParameter<bool> v_kuehler3;
		ofParameter<bool> v_kuehler4;
		ofParameter<bool> v_kuehler5;
		ofParameter<int> v_theremin;
		ofParameter<int> v_trichter_rot;
		ofParameter<int> v_trichter_mic;
		ofParameter<int> v_eieiei_spin;
		ofParameter<int> v_eieiei_membrane;
		ofParameter<int> v_goldenBox;
		ofParameter<int> v_bassfahrer;
		ofParameter<int> v_foehn;
		ofParameter<bool> v_foehn_schalter1;
		ofParameter<bool> v_foehn_schalter2;
		ofParameter<bool> v_foehn_schalter3;

};
