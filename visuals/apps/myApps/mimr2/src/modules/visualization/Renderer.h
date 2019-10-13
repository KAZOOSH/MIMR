#pragma once
#include "ofMain.h"
#include "RadarAttributes.h"
#include "Timer.h"
#include "ofxEasing.h"

class RendererSettings {
public:
	ofJson settings;
	shared_ptr<RadarAttributes> radar;

};

class Renderer
{
public:
	Renderer();
	~Renderer();
	void setup(RendererSettings settings);
	void draw();
	void loadShaders();

	//callbacks
	void onZoomStart(bool& isStart);
	void onColorInvasionStart(bool& isStart);
	void onLineShapeStart(bool& isStart);

protected:
	void renderZoom();
	void renderColorInvasion();
	void renderLineWidth();

	void drawEffectStartAnimation(string effectName);
	void calculateRingValues(string effectName, ofVec2f& center, float& radius);
	void drawRing(ofVec2f center, float radius, float width, ofColor color = ofColor(255));
	void drawOpenRing(float startAngle, float radius, float width, ofColor color = ofColor(255));

private:
	// settings
	ofJson settings;
	shared_ptr<RadarAttributes> radar;
	int width;
	int height;

	// shader
	ofShader waves;
	ofShader blending;
	ofShader shaper;
	ofShader chromaticAbberation;
	ofShader colorInvasion;
	int lastPos = 0;

	map<string, Timer> timer;

	// fbos
	ofFbo fboZoom;
	ofFbo fboRadar;
	ofFbo fboBgBlend;
};

