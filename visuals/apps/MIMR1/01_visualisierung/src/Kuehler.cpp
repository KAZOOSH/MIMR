//
//  GoldenBox.cpp
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//


#include "Kuehler.h"



void Kuehler::setup(RadarAttributes* params) {
    Layer::setup(params);
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    shader.load("shader/kuehler");
    img.allocate(1024,1024,OF_IMAGE_COLOR_ALPHA);
    
}

void Kuehler::draw() {
    ofEnableAlphaBlending();
    
    fbo.begin();
    ofClear(0, 0);
    shader.begin();
    shader.setUniform1f("speed", 2.0);
    shader.setUniform1f("angle", params->angle);
    shader.setUniform1f("time", params->time);
    shader.setUniform1i("kuehler1",params->v_kuehler1);
    shader.setUniform1i("kuehler2",params->v_kuehler2);
    shader.setUniform1i("kuehler3",params->v_kuehler3);
    shader.setUniform1i("kuehler4",params->v_kuehler4);
    shader.setUniform1i("kuehler5",params->v_kuehler5);
	shader.setUniform1i("colorSet", params->colorSet);
    ofSetColor(255);
    img.draw(0, 0);
    shader.end();
    fbo.end();
    
    fbo.draw(0,0);
    
    
}

void Kuehler::reload() {
    shader.load("shader/kuehler");
}