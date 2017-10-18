//
//  GoldenBox.cpp
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//


#include "Foehn.h"



void Foehn::setup(RadarAttributes* params) {
    Layer::setup(params);
    fbo.allocate(1024,1024,GL_RGBA32F_ARB);
    
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    shader.load("shader/billboard");
    
    /*billboards.getVertices().resize(1);
     billboards.getColors().resize(1);
     billboards.getNormals().resize(1,ofVec3f(0));*/
    
    billboards.setUsage( GL_DYNAMIC_DRAW );
    billboards.setMode(OF_PRIMITIVE_POINTS);
    
    
    decay = .01;
    
    ofDisableArbTex();
    for (int i=0; i<4; ++i) {
        textures.push_back(ofImage());
    }
    textures[0].load("dot.png");
    textures[1].load("line_v.png");
    textures[2].load("line_h.png");
    textures[3].load("cross.png");
    
    currentTexture = textures[0].getTexture();
    
    ofEnableAlphaBlending();
    
}

void Foehn::draw() {
    
    for (int i=0; i<map(params->v_foehn,0,127,0,100,1.0);++i)
    {
        createParticle();
    }
    updateParticles();
    
    ofDisableArbTex();
    ofEnableAlphaBlending();
    fbo.begin();
    ofEnablePointSprites();
    ofClear(0, 0);
    ofSetColor(255);
    shader.begin();
    currentTexture.bind();
    billboards.draw();
    currentTexture.unbind();
    shader.end();
    ofDisablePointSprites();
    fbo.end();
    ofEnableAlphaBlending();
    
    
    fbo.draw(0,0);
    
    
}

void Foehn::createParticle(){
    float a = params->toRad(params->angle);
    float radius = ofRandom(100, 300);
    particles.push_back(Particle());
    float offset = ofRandom(.1);
    particles.back().pos =ofVec2f(512 + cos(a-offset)*radius,512 + sin(a-offset)*radius);
    particles.back().color = 255;
    
    //ofVec2f r =ofVec2f(sin(a-offset)*radius, cos(a-offset)*radius
    
    particles.back().rotation = ofVec2f(-1,0).angle(ofVec2f(sin(a),cos(a)))*PI/180;
    //cout <<
    
    billboards.addVertex(ofVec3f(512 + cos(a-offset)*radius,512 + sin(a-offset)*radius,0));
    billboards.addColor(ofColor(255,255,255,255));
    billboards.addNormal(ofVec3f(4 + 3*params->v_foehn_schalter3,particles.back().rotation,0));
    
    ofVec2f v = ofVec2f(cos(a) + ofRandom(-.1,.1), sin(a)+ ofRandom(-.1,.1));
    particles.back().dir = v;
}

void Foehn::updateParticles(){
    if(particles.size()){
        for(int i=particles.size()-1; i>=0; --i){
            particles[i].color -= decay;
            
            particles[i].rotation += 0.1;
			int t = floor(particles[i].rotation);
            particles[i].rotation = t - particles[i].rotation;
            
            billboards.getVertices()[i] += ofVec3f(particles[i].dir);
            billboards.getColors()[i].a -= decay;
            billboards.getNormals()[i].y += 0.1;
            
            
            if (billboards.getColors()[i].a <= 0){
                particles.erase(particles.begin() + i);
                billboards.removeColor(i);
                billboards.removeVertex(i);
                billboards.removeNormal(i);
                
            }
        }
    }
    
    //set particle type
    if(params->v_foehn_schalter1 && params->v_foehn_schalter2){
        currentTexture = textures[3].getTexture();
    }
    else if(params->v_foehn_schalter1){
        currentTexture = textures[1].getTexture();
    }
    else if(params->v_foehn_schalter2){
        currentTexture = textures[2].getTexture();
    }
    else{
        currentTexture = textures[0].getTexture();
    }
    
}

void Foehn::reload() {
    shader.load("shader/billboard");
}