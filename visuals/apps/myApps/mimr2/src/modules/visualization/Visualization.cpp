#include "Visualization.h"
#include "GeneralPurposeFunctions.h"

namespace ofxModule {
    
	Visualization::Visualization(string moduleName):ModuleDrawable("Visualization",moduleName){
		
		ofSetWindowShape(1920, 1080);
		// register mouse events for interaction
		ofRegisterMouseEvents(this);
		ofRegisterKeyEvents(this);

		radarAttributes.setup(settings["midi"], settings["rendering"]);

		MidiCtrlSettings midiSettings;
		midiSettings.jsonSettings = settings;
		midiSettings.radarAttributes = shared_ptr<RadarAttributes>(&radarAttributes);
		midiCtrl.setup(midiSettings);

		gui.setup("params");
		gui.add(radarAttributes.params);

		RendererSettings rendererSettings;
		rendererSettings.radar = shared_ptr<RadarAttributes>(&radarAttributes);
		rendererSettings.settings = settings;
		renderer.setup(rendererSettings);

		ofBackground(0);

		rendering.allocate(settings["rendering"]["radarSize"][0], settings["rendering"]["radarSize"][1]);

		//osc.setup(settings["midi"]["chaosObjects"]["oscPort"].get<int>());


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
    }
  
    
    
    //------------------------------------------------------------------
    void Visualization::update() {
		midiCtrl.update();

		//// update chaos osc
		//// check for waiting messages
		//while (osc.hasWaitingMessages()) {

		//	// get the next message
		//	ofxOscMessage m;
		//	osc.getNextMessage(m);

		//	// check for mouse moved message
		//	if (m.getAddress() == "/chaos") {
		//		int id = m.getArgAsInt32(0);
		//		bool isActive = m.getArgAsBool(1);
		//		float x = m.getArgAsFloat(2);
		//		float y = m.getArgAsFloat(3);

		//		for (auto& chaos : radarAttributes.chaosObjects) {
		//			if (chaos.second.note == id) {
		//				chaos.second.positionXY = ofVec2f(x*rendering.getWidth(), y*rendering.getHeight());
		//				if (isActive && chaos.second.positionXY.distance(ofVec2f(rendering.getWidth(), rendering.getHeight())*.5) < rendering.getWidth()*0.5) {
		//					chaos.second.isActive = true;
		//					//cout << chaos.second.note << "  active " << chaos.second.isActive << "  pos" << x << "," << y << "     "<< chaos.second.positionXY<<endl;
		//				}
		//				else {
		//					chaos.second.isActive = false;
		//				}
		//			}
		//		}
		//	}

		//}

		// update chaos midi
		float maxAngle = 0.05;
		for (auto& object : radarAttributes.chaosObjects) {
			ofVec2f center = ofVec2f(rendering.getWidth(), rendering.getHeight())*.5;
			ofVec2f v0 = ofVec2f(1, 0);
			ofVec2f v1 = object.second.positionXY - center;
			v1.normalize();
			//if (object.second.isActive) cout << v1 << endl;
			float angle = v0.angleRad(v1);
			//if (angle < 0) angle = 360 + angle;
			//angle /= 180 * PI;

			if (angle < 0) angle = 2*PI + angle;
			//angle /= 180 * PI;

			//if (object.second.isActive) cout << angle*180/PI << "  " << radarAttributes.beat.getCurrentBeat().angle*180/PI << endl;

			float dAngle = abs(radarAttributes.beat.getCurrentBeat().angle - angle);
			bool isIn = dAngle <= maxAngle ? true : false;

			if (object.second.hasSend && !isIn) {
				
					midiCtrl.sendMidi(object.second.channel, object.second.note, 0);
		
				object.second.hasSend = false;
				//cout << 00"midi off " << object.second.channel << "  " << object.second.note << endl;
			}
			else if (object.second.isActive && isIn && !object.second.hasSend) {
	
					midiCtrl.sendMidi(object.second.channel, object.second.note, 127);
				
				
			
					object.second.hasSend = true;
					object.second.lastActivated = ofGetElapsedTimeMillis();


					cout << "midi on " << object.second.channel << "  " << object.second.note << endl;
			}
			
		}
    }

	void Visualization::draw()
	{
		ofSetColor(255);
		rendering.begin();
		ofClear(0, 0);
		renderer.draw();
		rendering.end();
		//rendering.draw((ofGetWidth()-rendering.getWidth())*0.5, 0);
		
		//======================== get our quad warp matrix.

		ofMatrix4x4 mat = warper.getMatrix();

		//======================== use the matrix to transform our fbo.

		
		ofPushMatrix();
		ofMultMatrix(mat);
		rendering.draw(0, 0);
		if (warper.isShowing()) {
			ofSetColor(255, 200);
			ofDrawCircle(rendering.getWidth() / 2, rendering.getHeight() / 2, rendering.getWidth() / 2);
			ofSetColor(0);
			ofDrawRectangle(0, rendering.getWidth() / 2, rendering.getWidth(), 2);
			ofDrawRectangle(rendering.getWidth() / 2,0,2, rendering.getWidth());
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
		
		if(isGui)gui.draw();


	}

	void Visualization::mousePressed(ofMouseEventArgs & mouse)
	{

	}
    
    
	void Visualization::keyPressed(ofKeyEventArgs & args)
	{
		if (args.key == 'w') {
			renderer.loadShaders();
		}

		if (args.key == 'd') {
			warper.toggleShow();
		}

		if (args.key == 'g' ) {
			isGui = !isGui;
		}

		if (args.key == 's') {
			warper.save();
		}
		if (args.key == 'f') {
			ofToggleFullscreen();
		}
	}

	//------------------------------------------------------------------
    void Visualization::proceedModuleEvent(ModuleEvent& e) {
		

    }
    
}
