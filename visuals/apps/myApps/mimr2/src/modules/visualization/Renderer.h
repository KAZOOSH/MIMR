#pragma once
#include "ofMain.h"
#include "RadarAttributes.h"

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
	void loadShader();

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
	int lastPos = 0;
};

