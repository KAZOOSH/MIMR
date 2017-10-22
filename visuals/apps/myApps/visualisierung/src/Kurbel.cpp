//
//  Kurbel.cpp
//  visualisierung
//
//  Created by Brian Eschrich on 10.10.17.
//
//

#include "Kurbel.h"

void Kurbel::setup(RadarAttributes* params) {
    Layer::setup(params);
    
    ofEnableArbTex();
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    valueFbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    valueFbo.begin();
    ofClear(255,255,255, 0);
    valueFbo.end();
    
    
    shader.load("shader/kurbel");
    
    img.allocate(1024,1024,OF_IMAGE_COLOR_ALPHA);
    
    radius = 10;
    distance = 480;
}

void Kurbel::draw() {
    ofEnableAlphaBlending();
    //cout << ofNoise(0.1) <<endl;
    
    //draw values
    float pStart = params->lastAngle *STEPSIZE_TEX;
    
    float nSteps = params->angle - params->lastAngle;
    if (params->lastAngle > params->angle) {
        nSteps = 360 - params->lastAngle + params->angle;
    }
    nSteps *= STEPSIZE_TEX;
    //cout << nSteps <<endl;
    
    ofEnableArbTex();
    valueFbo.begin();
    ofSetColor(params->v_kurbel*2);
    for (int i=pStart; i<pStart+nSteps; ++i) {
        int px = i%1024;
        int py = i/1024*3;
        ofDrawRectangle(px, py, 1, 3);
    }
    valueFbo.end();
    
    
    fbo.begin();
    ofClear(0, 0);
    shader.begin();
    shader.setUniform1f("speed", 2.0);
    shader.setUniform1f("angle", params->angle);
    shader.setUniform1f("time", params->time);
	shader.setUniform1i("colorSet", params->colorSet);
    //shader.setUniformTexture("tex0", fbo.getTexture(), 1);
    shader.setUniformTexture("tex0", valueFbo.getTexture(), 1);
    img.draw(0,0);
    shader.end();
    fbo.end();
    
    fbo.draw(0,0);
    //valueFbo.draw(0,50,4096,4096);
    
}


void Kurbel::reload() {
    shader.load("shader/kurbel");
}