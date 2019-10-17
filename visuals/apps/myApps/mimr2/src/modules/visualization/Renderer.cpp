#include "Renderer.h"



Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::setup(RendererSettings settings_)
{
	ofEnableAntiAliasing();

	settings = settings_.settings;
	radar = settings_.radar;
	width = settings["rendering"]["radarSize"][0];
	height = settings["rendering"]["radarSize"][1];

	ofSetCircleResolution(400);

	fboRadar.allocate(width, height);
	fboRadar.begin();
	ofClear(0, 0);
	fboRadar.end();

	fboZoom.allocate(width, height);
	fboZoom.begin();
	ofClear(0, 0);
	fboZoom.end();

	fboBgBlend.allocate(width, height);
	fboBgBlend.begin();
	ofClear(0, 0);
	fboBgBlend.end();

	fboDistort.allocate(width, height);
	fboDistort.begin();
	ofClear(0, 0);
	fboDistort.end();

	// load shaders
	loadShaders();

	timer.insert(pair<string, Timer>("zoom", Timer()));
	timer["zoom"].setRounds(3);
	timer["zoom"].setCountdown(1000, 0); // enter
	timer["zoom"].setCountdown(1500, 1); // zoom
	timer["zoom"].setCountdown(700, 2); // fadeOut

	timer.insert(pair<string, Timer>("colorInvasion", Timer()));
	//timer["colorInvasion"].setRounds(2);
	timer["colorInvasion"].setCountdown(2000, 0); // enter
	//timer["colorInvasion"].setCountdown(700, 1); // fadeOut

	timer.insert(pair<string, Timer>("lineShape", Timer()));
	timer["lineShape"].setCountdown(2000, 0); // enter

	timer.insert(pair<string, Timer>("lineWidth", Timer()));
	timer["lineWidth"].setCountdown(2000, 0); // enter

	timer.insert(pair<string, Timer>("optic", Timer()));
	timer["optic"].setCountdown(2000, 0); // enter

	for (auto& instrument : radar->instruments) {
		string effect = instrument.second.effect;
		if (effect == "zoom") {
			instrument.second.isActive.addListener(this, &Renderer::onZoomStart);
		}else if (effect == "colorInvasion") {
			instrument.second.isActive.addListener(this, &Renderer::onColorInvasionStart);
		}else if (effect == "lineShape") {
			instrument.second.isActive.addListener(this, &Renderer::onLineShapeStart);
		}else if (effect == "lineWidth") {
			instrument.second.isActive.addListener(this, &Renderer::onLineWidthStart);
		}else if (effect == "optic") {
			instrument.second.isActive.addListener(this, &Renderer::onDistortionStart);
		}
	}


}

void Renderer::draw()
{
	for (auto&t : timer) {
		t.second.update();
	}

	

	renderWaves();
	renderColorInvasion();
	renderDistortion();
	renderZoom();

	for (auto& e : radar->chaosObjects) {
		ofSetColor(255);
		if (e.second.isActive) {
			float y = height - e.second.positionXY.y;
			ofDrawCircle(e.second.positionXY.x,y, 10);
		}
	}

	//radar->chaosObjects[1].fboWave.draw(0, 0);

	

	//fboRadar.draw(800, 0);

	//radar->instruments[3].fboTex.draw(0, 600, 600, 600);
	//radar->instruments[3].fboWave.draw(600, 0, 600, 600);
	//radar->instruments[3].fboAbberation.draw(600, 600, 600, 600);

	
}

void Renderer::loadShaders()
{
	waves.load("shaders/waves");
	blending.load("shaders/blending");
	shaper.load("shaders/shaper");
	chromaticAbberation.load("shaders/chromaticAbberation");
	colorInvasion.load("shaders/colorInvasion");
	distortion.load("shaders/distortion");
}

void Renderer::onZoomStart(bool & isStart)
{
	if (isStart) { 
		timer["zoom"].setCountdown(radar->beat.getBeatLength(), 0);
		timer["zoom"].start(); 
	}
	else { timer["zoom"].stop(); }
	
}

void Renderer::onColorInvasionStart(bool & isStart)
{
	if (isStart) { 
		timer["colorInvasion"].setCountdown(radar->beat.getBeatLength()*2, 0);
		timer["colorInvasion"].setCountdown(radar->beat.getBeatLength() * 4, 1);
		timer["colorInvasion"].start(); 
	}
	else { timer["colorInvasion"].stop(); }
}

void Renderer::onLineShapeStart(bool & isStart)
{
	if (isStart) {
		timer["lineShape"].setCountdown(radar->beat.getBeatLength()*2, 0);
		timer["lineShape"].start();
	}
	else { timer["lineShape"].stop(); }
}

void Renderer::onLineWidthStart(bool & isStart)
{
	if (isStart) {
		timer["lineWidth"].setCountdown(radar->beat.getBeatLength()*2, 0);
		timer["lineWidth"].start();
	}
	else { timer["lineWidth"].stop(); }
}

void Renderer::onDistortionStart(bool & isStart)
{
	if (isStart) {
		timer["optic"].setCountdown(radar->beat.getBeatLength() * 2, 0);
		timer["optic"].start();
	}
	else { timer["optic"].stop(); }
}

void Renderer::renderWaves()
{
	
	int pos = fmod(ofGetElapsedTimeMillis() / 7, width);
	int i = 0;
	float wRadar = 2;

	// waves
	for (auto& instrument : radar->instruments) {
		if (instrument.second.effect == "lineDistance") {
			//create wave	
			instrument.second.fboTex.begin();

			ofColor c = ofColor(0);
			ofSetColor(c);
			int w = instrument.second.fboTex.getWidth();
			int h = instrument.second.fboTex.getHeight();

			if (lastPos > pos) {
				ofDrawRectangle(lastPos, 0, 100, h);
				ofDrawRectangle(0, 0, pos, h);
			}
			else {
				ofDrawRectangle(lastPos, 0, pos - lastPos, h);
			}

			float dLastIndex = pos - instrument.second.posLastRadarWave;
			if (instrument.second.posLastRadarWave > pos) {
				dLastIndex = w - instrument.second.posLastRadarWave + pos;
			}

			int val = 0;

			if (instrument.second.isActive) {
				for (auto& v : instrument.second.values) {
					val += v;
				}
				val /= instrument.second.values.size();
			}
			
			if (val != 0) {
				float minDToDraw = ofMap(val, 1, 127, 400, 50);
				if (minDToDraw <= dLastIndex) {
					ofSetColor(255);
					int drawPos = pos - wRadar;
					if (drawPos < 0) {
						ofDrawRectangle(w + drawPos, 0, abs(drawPos), h);
						ofDrawRectangle(0, 0, wRadar + drawPos, h);
					}
					else {
						ofDrawRectangle(drawPos, 0, wRadar, h);
					}
					instrument.second.posLastRadarWave = pos;
				}
			}


			instrument.second.fboTex.end();

			//shape wave
			auto& iShape = radar->instrumentByEffect["lineShape"][0];

			if (iShape->isActive) {
				instrument.second.fboShaper.begin();
				ofClear(0, 0);
				shaper.begin();
				shaper.setUniformTexture("wave", instrument.second.fboTex.getTexture(), 1);
				shaper.setUniform1i("width", instrument.second.fboShaper.getWidth());
				shaper.setUniform1i("height", instrument.second.fboShaper.getHeight());
				shaper.setUniform1f("time", pos);
				shaper.setUniform1i("shaper0", iShape->values[1] == 127 ? 1 : 0);
				shaper.setUniform1i("shaper1", iShape->values[2] == 127 ? 1 : 0);
				shaper.setUniform1i("shaper2", iShape->values[3] == 127 ? 1 : 0);
				shaper.setUniform1f("intensity", ofMap(iShape->values[0], 0, 127, 0, 1));
				ofDrawRectangle(0, 0, instrument.second.fboShaper.getWidth(), instrument.second.fboTex.getHeight());
				shaper.end();
				instrument.second.fboShaper.end();
			}


			//map wave in circle
			float angle = float(instrument.second.position) / 360 * 2.0 * PI;
			float angleShaper = float(iShape->position) / 360 * 2.0 * PI;

			float radiusShaper = 0;
			if (iShape->isActive) {
				if (timer["lineShape"].isRunning()) {
					radiusShaper = ofMap(timer["lineShape"].getPercentage(), 0, 1, 0, width);
				}
				else {
					radiusShaper = width;
				}
			}

			auto iWidth = radar->instrumentByEffect["lineWidth"][0];
			float angleWidth = float(iWidth->position) / 360 * 2.0 * PI;
			float radiusWidth = 0;
			if (iWidth->isActive) {
				if (timer["lineWidth"].isRunning()) {
					radiusWidth = ofMap(timer["lineWidth"].getPercentage(), 0, 1, 0, width);
				}
				else {
					radiusWidth = width;
				}
			}

			instrument.second.fboWave.begin();
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			ofClear(0, 0);
			waves.begin();
			waves.setUniformTexture("wave", instrument.second.fboTex.getTexture(), 1);
			waves.setUniformTexture("shaper", instrument.second.fboShaper.getTexture(), 2);
			waves.setUniform1i("width", width);
			waves.setUniform1i("height", height);
			waves.setUniform1f("radius", width);
			waves.setUniform1f("rShaper", radiusShaper);
			waves.setUniform2f("centerShaper", ofVec2f(width*0.5 + cos(angleShaper)*width*0.5, width*0.5 + sin(angleShaper) * width*0.5));
			waves.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
			waves.setUniform1f("time", pos);
			waves.setUniform1i("lineWidth", ofMap(iWidth->values[0], 0, 127, 0.0, 20));
			waves.setUniform1f("rLineWidth", radiusWidth);
			waves.setUniform2f("centerLineWidth", ofVec2f(width*0.5 + cos(angleWidth)*width*0.5, width*0.5 + sin(angleWidth) * width*0.5));

			ofDrawRectangle(0, 0, instrument.second.fboTex.getWidth(), instrument.second.fboTex.getHeight());
			waves.end();


			instrument.second.fboWave.end();
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			instrument.second.fboAbberation.begin();
			ofClear(0, 0);
			chromaticAbberation.begin();
			chromaticAbberation.setUniformTexture("tex", instrument.second.fboWave.getTexture(), 1);
			chromaticAbberation.setUniform1i("width", instrument.second.fboWave.getWidth());
			chromaticAbberation.setUniform1i("height", instrument.second.fboWave.getHeight());
			chromaticAbberation.setUniform1f("radius", width);
			chromaticAbberation.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
			chromaticAbberation.setUniform1f("time", pos);
			chromaticAbberation.setUniform1f("maxAbberation", 4.0f);

			ofDrawRectangle(0, 0, instrument.second.fboWave.getWidth(), instrument.second.fboWave.getHeight());

			chromaticAbberation.end();


			instrument.second.fboAbberation.end();

			//cout << ofVec2f(shaderWidth*0.5 + cos(angle)*shaderWidth*0.5, shaderWidth*0.5 + sin(angle) * shaderWidth*0.5) << endl;
			i++;
		}
	}

	// chaos objects
	for (auto& instrument : radar->chaosObjects) {
			//create wave	
			instrument.second.fboTex.begin();

			ofColor c = ofColor(0);
			ofSetColor(c);
			int w = instrument.second.fboTex.getWidth();
			int h = instrument.second.fboTex.getHeight();

			if (lastPos > pos) {
				ofDrawRectangle(lastPos, 0, 100, h);
				ofDrawRectangle(0, 0, pos, h);
			}
			else {
				ofDrawRectangle(lastPos, 0, pos - lastPos, h);
			}

			float dLastIndex = pos - instrument.second.posLastRadarWave;
			if (instrument.second.posLastRadarWave > pos) {
				dLastIndex = w - instrument.second.posLastRadarWave + pos;
			}

			int val = instrument.second.hasSend ? 127 : 0;

			if (val != 0) {
				//float minDToDraw = ofMap(val, 1, 127, 400, 50);
				float minDToDraw = 2;
				if (minDToDraw <= dLastIndex) {
					ofSetColor(255);
					int drawPos = pos - wRadar;
					if (drawPos < 0) {
						ofDrawRectangle(w + drawPos, 0, abs(drawPos), h);
						ofDrawRectangle(0, 0, wRadar + drawPos, h);
					}
					else {
						ofDrawRectangle(drawPos, 0, wRadar, h);
					}
					instrument.second.posLastRadarWave = pos;
				}
			}


			instrument.second.fboTex.end();

			//shape wave
			auto& iShape = radar->instrumentByEffect["lineShape"][0];

			if (iShape->isActive) {
				instrument.second.fboShaper.begin();
				ofClear(0, 0);
				shaper.begin();
				shaper.setUniformTexture("wave", instrument.second.fboTex.getTexture(), 1);
				shaper.setUniform1i("width", instrument.second.fboShaper.getWidth());
				shaper.setUniform1i("height", instrument.second.fboShaper.getHeight());
				shaper.setUniform1f("time", pos);
				shaper.setUniform1i("shaper0", iShape->values[1] == 127 ? 1 : 0);
				shaper.setUniform1i("shaper1", iShape->values[2] == 127 ? 1 : 0);
				shaper.setUniform1i("shaper2", iShape->values[3] == 127 ? 1 : 0);
				shaper.setUniform1f("intensity", ofMap(iShape->values[0], 0, 127, 0, 1));
				ofDrawRectangle(0, 0, instrument.second.fboShaper.getWidth(), instrument.second.fboTex.getHeight());
				shaper.end();
				instrument.second.fboShaper.end();
			}


			//map wave in circle
			float angleShaper = float(iShape->position) / 360 * 2.0 * PI;

			float radiusShaper = 0;
			if (iShape->isActive) {
				if (timer["lineShape"].isRunning()) {
					radiusShaper = ofMap(timer["lineShape"].getPercentage(), 0, 1, 0, width);
				}
				else {
					radiusShaper = width;
				}
			}

			auto iWidth = radar->instrumentByEffect["lineWidth"][0];
			float angleWidth = float(iWidth->position) / 360 * 2.0 * PI;
			float radiusWidth = 0;
			if (iWidth->isActive) {
				if (timer["lineWidth"].isRunning()) {
					radiusWidth = ofMap(timer["lineWidth"].getPercentage(), 0, 1, 0, width);
				}
				else {
					radiusWidth = width;
				}
			}

			ofVec2f posCenter(instrument.second.positionXY.x, height - instrument.second.positionXY.y);

			instrument.second.fboWave.begin();
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			ofClear(0, 0);
			waves.begin();
			waves.setUniformTexture("wave", instrument.second.fboTex.getTexture(), 1);
			waves.setUniformTexture("shaper", instrument.second.fboShaper.getTexture(), 2);
			waves.setUniform1i("width", width);
			waves.setUniform1i("height", height);
			waves.setUniform1f("radius", width);
			waves.setUniform1f("rShaper", radiusShaper);
			waves.setUniform2f("centerShaper", ofVec2f(width*0.5 + cos(angleShaper)*width*0.5, width*0.5 + sin(angleShaper) * width*0.5));
			waves.setUniform2f("center", posCenter);
			waves.setUniform1f("time", pos);
			waves.setUniform1i("lineWidth", ofMap(iWidth->values[0], 0, 127, 0.0, 20));
			waves.setUniform1f("rLineWidth", radiusWidth);
			waves.setUniform2f("centerLineWidth", ofVec2f(width*0.5 + cos(angleWidth)*width*0.5, width*0.5 + sin(angleWidth) * width*0.5));

			ofDrawRectangle(0, 0, instrument.second.fboTex.getWidth(), instrument.second.fboTex.getHeight());
			waves.end();


			instrument.second.fboWave.end();
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

			instrument.second.fboAbberation.begin();
			ofClear(0, 0);
			chromaticAbberation.begin();
			chromaticAbberation.setUniformTexture("tex", instrument.second.fboWave.getTexture(), 1);
			chromaticAbberation.setUniform1i("width", instrument.second.fboWave.getWidth());
			chromaticAbberation.setUniform1i("height", instrument.second.fboWave.getHeight());
			chromaticAbberation.setUniform1f("radius", width);
			chromaticAbberation.setUniform2f("center", posCenter);
			chromaticAbberation.setUniform1f("time", pos);
			chromaticAbberation.setUniform1f("maxAbberation", 4.0f);

			ofDrawRectangle(0, 0, instrument.second.fboWave.getWidth(), instrument.second.fboWave.getHeight());

			chromaticAbberation.end();


			instrument.second.fboAbberation.end();

			//cout << ofVec2f(shaderWidth*0.5 + cos(angle)*shaderWidth*0.5, shaderWidth*0.5 + sin(angle) * shaderWidth*0.5) << endl;
			i++;
		
	}



	lastPos = pos;

	fboRadar.begin();
	ofClear(0, 0);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	ofSetColor(255);
	blending.begin();
	blending.setUniform1i("width", width);
	blending.setUniform1i("height", height);
	blending.setUniform1f("time", pos);
	i = 0;
	for (auto& instrument : radar->instruments) {
		if (instrument.second.effect == "lineDistance") {
			blending.setUniformTexture("tex" + ofToString(i), instrument.second.fboAbberation.getTexture(), i + 1);
			i++;
		}
	}
	for (auto& chaos : radar->chaosObjects) {
		blending.setUniformTexture("tex" + ofToString(i), chaos.second.fboAbberation.getTexture(), i + 1);
		i++;

	}
	ofDrawRectangle(0, 0, width + 200, width);

	blending.end();


	// radar line
	ofVec2f p1(width*.5, height*.5);
	ofVec2f p2(width*.5 + cos(radar->beat.getCurrentBeat().angle)*width*0.5, height*.5 - sin(radar->beat.getCurrentBeat().angle)*height*0.5);
	ofVec2f d = p2 - p1;
	ofDrawLine(p1, p2);
	float percentage = 1 - (fmod(radar->beat.getCurrentBeat().beat,16)/16.0);
	ofDrawCircle(p1 + d * percentage,ofxeasing::map(percentage,0,1,2,10,ofxeasing::sine::easeInOut));

	fboRadar.end();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	
}

void Renderer::renderZoom()
{
	// start animation
	float zoomValue = 1.0;
	fboZoom.begin();
	ofClear(0, 0);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	if (timer["zoom"].isRunning()) {
		if (timer["zoom"].getRound() == 0) { // start
			drawEffectStartAnimation("zoom");
		}else if (timer["zoom"].getRound() == 1){ // zoom
			float r;
			float angle;
			r = width / 2;
			ofVec2f center = ofVec2f(r, r);

			float percentage = sin(timer["zoom"].getPercentage() * PI); // to curve
			percentage = ofxeasing::map(percentage, 0.0, 1.0, 1.0, 0.8, ofxeasing::linear::easeIn); // animation

			float radius = percentage * r;
			float width = 10;
			drawRing(center, radius, width);
			zoomValue = percentage;
		}else { //fadeOut
			ofColor c = ofColor(255);
			float wRing =ofMap(timer["zoom"].getPercentage(),0,1,10,5);
			drawRing(ofVec2f(width/2), width/2, wRing,c);
		}
	}
	// normal animation
	else if (radar->instrumentByEffect["zoom"][0]->isActive) {
			zoomValue = ofMap(radar->instrumentByEffect["zoom"][0]->values[0], 0, 127, 1.0, 0.5);
			drawRing(ofVec2f(width / 2), width / 2* zoomValue, 5, ofColor(255));
	}

	
	
	fboZoom.end();
	ofPushMatrix();
	float d = ofMap(zoomValue, 1.0, 0.0, 0.0, width*0.5);
	ofTranslate(d, d);
	//fboBgBlend.draw(0, 0, width*zoomValue, height*zoomValue);
	fboDistort.draw(0, 0, width*zoomValue, height*zoomValue);
	ofPopMatrix();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	fboZoom.draw(0, 0);
}

void Renderer::renderColorInvasion()
{
	// start animation
	fboBgBlend.begin();

	ofClear(0, 0);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	if (radar->instrumentByEffect["colorInvasion"][0]->isActive) { //draw colored bg
		ofVec2f center = ofVec2f(width*0.5);
		float radius = width*0.5;
		if (timer["colorInvasion"].getRound() == 0) {
			calculateRingValues("colorInvasion", center, radius);
		}
		float angle = radar->instrumentByEffect["colorInvasion"][0]->position * PI/180;
		colorInvasion.begin();
		//chromaticAbberation.setUniformTexture("tex", instrument.second.fboWave.getTexture(), 1);
		colorInvasion.setUniform1i("width",width);
		colorInvasion.setUniform1i("height", width);
		colorInvasion.setUniform1f("radius", width);
		colorInvasion.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
		ofMap(radar->instrumentByEffect["colorInvasion"][0]->values[0], 0, 127, 0, 1);
		colorInvasion.setUniform1f("scaleColor", ofMap(radar->instrumentByEffect["colorInvasion"][0]->values[0],0,127,-0.5,0.5));
		colorInvasion.setUniform1f("noiseStrength", ofMap(radar->instrumentByEffect["colorInvasion"][0]->values[1], 0, 127, 0, 1.0));
		colorInvasion.setUniform1f("time", radar->beat.getCurrentBeat().beat);
		//chromaticAbberation.setUniform1f("maxAbberation", 4.0f);


		ofDrawCircle(center, radius);
		colorInvasion.end();
	}
	
	ofSetColor(255);
	fboRadar.draw(0, 0);

	renderLineShape();
	renderLineWidth();

	fboBgBlend.end();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::renderLineShape()
{
	if (timer["lineShape"].getRound() == 0 && timer["lineShape"].isRunning()) {
		auto& iShape = radar->instrumentByEffect["lineShape"][0];
		float angle = iShape->position / 180.0 * PI;

		float r = width / 2;

		float percentage = ofxeasing::map(timer["lineShape"].getPercentage(), 0.0, 1.0, 0.0, 1.0, ofxeasing::sine::easeIn);
		drawRing(ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5), percentage*width, 3);
	}
}

void Renderer::renderLineWidth()
{
}

void Renderer::renderDistortion()
{
	// start animation
	fboDistort.begin();

	ofClear(0, 0);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	float radius = 0;
	if (radar->instrumentByEffect["optic"][0]->isActive) {
		if (timer["optic"].isRunning()) {
			radius = ofMap(timer["optic"].getPercentage(), 0, 1, 0, width);
		}
		else {
			radius = width;
		}
	}
	float angle = radar->instrumentByEffect["optic"][0]->position * PI / 180;

		distortion.begin();
		distortion.setUniformTexture("tex", fboBgBlend.getTexture(), 1);
		distortion.setUniform1i("width", width);
		distortion.setUniform1i("height", width);
		distortion.setUniform1f("radius", radius);
		distortion.setUniform1f("intensity", ofMap(radar->instrumentByEffect["optic"][0]->values[0],0,127,0,1));
		distortion.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
		ofDrawRectangle(0, 0, fboDistort.getWidth(), fboDistort.getHeight());

		//ofDrawCircle(center, radius);
		distortion.end();

	fboDistort.end();
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

}

void Renderer::drawEffectStartAnimation(string effectName)
{
	if (timer[effectName].isRunning()) {

		int lineWidth = 10;

		ofVec2f center;
		float radius;

		float angle = float(radar->instrumentByEffect[effectName][0]->position) / 360 * 2.0 * PI;
		float percentage = ofxeasing::map(timer[effectName].getPercentage(), 0.0, 1.0, 0.0, 1.0, ofxeasing::sine::easeIn);

		drawOpenRing(angle, percentage, lineWidth);

		//calculateRingValues(effectName, center, radius);
		//drawRing(center, radius, lineWidth);
	}


	
}

void Renderer::calculateRingValues(string effectName, ofVec2f & center, float & radius)
{
	float r = width / 2;
	float angle = float(radar->instrumentByEffect[effectName][0]->position) / 360 * 2.0 * PI;

	float percentage = ofxeasing::map(timer[effectName].getPercentage(), 0.0, 1.0, 0.0, 1.0, ofxeasing::sine::easeIn);
	float dCenter = (1.0 - percentage)*r;
	center = ofVec2f(r + cos(angle)*dCenter, r + sin(angle)*dCenter);
	radius = percentage * r;
}

void Renderer::drawRing(ofVec2f center, float radius, float width,ofColor color)
{
	ofPath ring;
	ring.setColor(color);
	ring.setCircleResolution(400);
	ring.circle(center, radius);
	ring.circle(center, radius - width);
	ring.close();
	ring.draw();
}

void Renderer::drawOpenRing(float startAngle, float radius, float width, ofColor color)
{
	ofVec2f center(height / 2, height / 2);
	float r = height / 2;

	ofPath ring;
	ring.setColor(color);
	ring.setCircleResolution(400);
	ring.arc(center, r, r, startAngle - radius * 180, startAngle + radius * 180);
	ring.arcNegative(center, r-width, r-width, startAngle + radius * 180, startAngle - radius * 180);
	//ring.circle(center, r - width);
	ring.close();
	ring.draw();
}
