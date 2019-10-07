#include "Renderer.h"



Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::setup(RendererSettings settings_)
{
	settings = settings_.settings;
	radar = settings_.radar;
	width = settings["rendering"]["radarSize"][0];
	height = settings["rendering"]["radarSize"][1];

	// load shaders
	loadShaders();
}

void Renderer::draw()
{
	int pos = fmod(ofGetElapsedTimeMillis() / 5, width);
	int i = 0;
	for(auto& instrument : radar->instruments){

		//create wave	
		instrument.second.fboTex.begin();
		ofColor c = ofColor(0);
		if (instrument.second.isActive) {
			c = ofColor(instrument.second.values[0]*2);
		}
		ofSetColor(c);
		
		if (lastPos > pos) {
			ofDrawRectangle(lastPos, 0, 100, instrument.second.fboTex.getHeight());
			ofDrawRectangle(0, 0, pos, instrument.second.fboTex.getHeight());
		}
		else {
			ofDrawRectangle(lastPos, 0, pos - lastPos, instrument.second.fboTex.getHeight());
		}

		instrument.second.fboTex.end();

		//shape wave
		instrument.second.fboShaper.begin();
		ofClear(0, 0);
		shaper.begin();
		shaper.setUniformTexture("wave", instrument.second.fboTex.getTexture(), 1);
		shaper.setUniform1i("width", instrument.second.fboShaper.getWidth());
		shaper.setUniform1i("height", instrument.second.fboShaper.getHeight());
		shaper.setUniform1f("time", pos);
		shaper.setUniform1i("shaper", 0);
		ofDrawRectangle(0, 0, instrument.second.fboShaper.getWidth(), instrument.second.fboTex.getHeight());

		shaper.end();
		instrument.second.fboShaper.end();

		//map wave in circle
		float angle = float(instrument.second.position) / float(radar->instruments.size()) * 2.0 * PI;

		instrument.second.fboWave.begin();
		ofClear(0, 0);
		waves.begin();
		waves.setUniformTexture("wave", instrument.second.fboShaper.getTexture(), 1);
		waves.setUniform1i("width", instrument.second.fboShaper.getWidth());
		waves.setUniform1i("height", instrument.second.fboShaper.getHeight());
		waves.setUniform1f("radius", width);
		//waves.setUniform1f("radius", 10);
		//waves.setUniform2f("center", ofVec2f(0 , shaderWidth*0.5));
		waves.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
		waves.setUniform1f("time", pos);

		ofDrawRectangle(0, 0, instrument.second.fboTex.getWidth(), instrument.second.fboTex.getHeight());
		waves.end();
		instrument.second.fboWave.end();

		instrument.second.fboAbberation.begin();
		ofClear(0, 0);
		chromaticAbberation.begin();
		chromaticAbberation.setUniformTexture("tex", instrument.second.fboWave.getTexture(), 1);
		chromaticAbberation.setUniform1i("width", instrument.second.fboWave.getWidth());
		chromaticAbberation.setUniform1i("height", instrument.second.fboWave.getHeight());
		chromaticAbberation.setUniform1f("radius", width);
		chromaticAbberation.setUniform2f("center", ofVec2f(width*0.5 + cos(angle)*width*0.5, width*0.5 + sin(angle) * width*0.5));
		chromaticAbberation.setUniform1f("time", pos);
		chromaticAbberation.setUniform1f("maxAbberation", 10.0f);

		ofDrawRectangle(0, 0, instrument.second.fboWave.getWidth(), instrument.second.fboWave.getHeight());

		chromaticAbberation.end();
		instrument.second.fboAbberation.end();

		//cout << ofVec2f(shaderWidth*0.5 + cos(angle)*shaderWidth*0.5, shaderWidth*0.5 + sin(angle) * shaderWidth*0.5) << endl;
		i++;
	}
	lastPos = pos;

	blending.begin();

	blending.setUniform1i("width", width);
	blending.setUniform1i("height", height);
	blending.setUniform1f("time", pos);

	i = 0;
	for (auto& instrument : radar->instruments) {
		blending.setUniformTexture("tex" + ofToString(i), instrument.second.fboAbberation.getTexture(), i + 1);
		i++;
	}

	ofDrawRectangle(0, 0, width + 200, width);

	blending.end();
	//radar->instruments[0].fboFinal.draw(0, 0, 600, 600);
	//instruments[0].fboShaper.draw(600, 0,600,600);

	//radar->instruments[3].fboTex.draw(600, 600,600,600);
}

void Renderer::loadShaders()
{
	waves.load("shaders/waves");
	blending.load("shaders/blending");
	shaper.load("shaders/shaper");
	chromaticAbberation.load("shaders/chromaticAbberation");
}
