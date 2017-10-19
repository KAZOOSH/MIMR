//
//  GoldenBox.cpp
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//


#include "Universum.h"



void Universum::setup(RadarAttributes* params) {
    Layer::setup(params);
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    shader.load("shader/universum");
    img.allocate(1024,1024,OF_IMAGE_COLOR_ALPHA);
    
    
}

void Universum::draw() {
    ofEnableAlphaBlending();
    
    fbo.begin();
    ofClear(0, 0);
    shader.begin();
    shader.setUniform1f("angle", params->angle);
    shader.setUniform1f("time", params->time);
    shader.setUniform1f("spiral", map(params->v_eieiei_spin,0,127,0.5,0.05,0.8));
    shader.setUniform1f("detail", map(params->v_eieiei_membrane,0,127,0.53,0.57,1.0));
    shader.setUniform1f("center", map(params->v_trichter_mic,0,127,0.0,1.0,1.0));
    shader.setUniform1f("centerBlur", map(params->v_trichter_rot,0,127,1.0,0.0,1.0));
    ofSetColor(255);
    img.draw(0, 0);
    shader.end();
    fbo.end();
    
    fbo.draw(0,0);
    
    
    
}

void Universum::reload() {
    shader.load("shader/universum");
}