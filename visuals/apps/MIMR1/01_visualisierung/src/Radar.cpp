//
//  GoldenBox.cpp
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//


#include "Radar.h"



void Radar::setup(RadarAttributes* params) {
    Layer::setup(params);
    
    ofSeedRandom();
    
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    rMin = 50;
    rMax = 512;
}

void Radar::draw() {
    ofEnableAlphaBlending();
    
    float a = params->toRad(params->angle);
    float px0 = 512 + cos(a)*rMin;
    float py0 = 512 + sin(a)*rMin;
    float px1 = 512 + cos(a)*rMax;
    float py1 = 512 + sin(a)*rMax;
    
    
    fbo.begin();
    ofFill();
    ofClear(0,0,0,0);
    ofSetColor(255,255,255, 255);
    
    int nElements = params->v_theremin/20;
    int nSegments = 10;
    ofVec2f begin = ofVec2f(px0,py0);
    ofVec2f start = ofVec2f(px0,py0);
    ofVec2f part = (ofVec2f(px1,py1)-start)/nSegments;
    
	ofColor color;
	if (params->colorSet == 0)
	{
		color = ofColor(150, 200, 255);
	}
	else if (params->colorSet == 1)
	{
		color = ofColor(135, 255, 27);
	}
	else if (params->colorSet == 2)
	{
		color = ofColor(220, 220, 255);
	}
	else if (params->colorSet == 3)
	{
		color = ofColor(136, 255, 219);
	}


    for (int i=0; i<nSegments; ++i) {
        ofSetColor(color);
        ofVec2f end = start + part;
        end.x += ofRandom(-params->v_theremin/20,params->v_theremin/20);
        end.y += ofRandom(-params->v_theremin/20,params->v_theremin/20);
        ofDrawLine(start.x, start.y, end.x, end.y);
        start = end;
    }
    
    for(int e=0; e<nElements; ++e){
        start = begin;
        for (int i=0; i<nSegments; ++i) {
            ofSetColor(color.r,color.g,color.b, 130);
            ofVec2f end = start + part;
            end.x += ofRandom(-params->v_theremin/10,params->v_theremin/10 );
            end.y += ofRandom(-params->v_theremin/10,params->v_theremin/10 );
            ofDrawLine(start.x, start.y, end.x, end.y);
            start = end;
        }
    }
    
    
    
    //ofDrawLine(px0, py0, px1, py1);
    
    
    
    
    
    fbo.end();
    
    fbo.draw(0,0);
    
    
}