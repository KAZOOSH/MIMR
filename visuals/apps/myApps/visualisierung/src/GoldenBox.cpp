//
//  GoldenBox.cpp
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//


#include "GoldenBox.h"



void GoldenBox::setup(RadarAttributes* params) {
    Layer::setup(params);
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    rMin = 150;
    rMax = 400;
}

void GoldenBox::draw() {
    ofEnableAlphaBlending();
    
    float rTMax = (rMax-rMin)/11 * params->v_goldenBox + rMin;
    
    //define Path
    ofPath abc;
    
    abc.arc( 512, 512, rTMax, rTMax, params->lastAngle, params->angle);
    abc.arcNegative( 512, 512, rMin, rMin, params->angle, params->lastAngle);
    abc.close();
    
    fbo.begin();
    ofFill();
    ofSetColor(255,255,255, 1);
    ofDrawRectangle(0,0,1024,1024);
    
    //ofPushMatrix();
    //ofTranslate(512,512);
    abc.draw();
    //ofPopMatrix();
    
    
    
    
    
    fbo.end();
    
    fbo.draw(0,0);
    
   /* ofSetColor(1*255,0,0.12*255);
    ofDrawCircle(512, 512, 80);
    ofSetColor(200,200,200);
    ofDrawCircle(512, 512, 75);*/
    
    
}