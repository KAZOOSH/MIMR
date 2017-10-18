//
//  GoldenBox.h
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "Layer.h"

class Particle{
public:
    ofVec2f pos;
    ofVec2f dir;
    float color;
    float rotation;
};

class Foehn: public Layer{
    
public:
    
    void setup(RadarAttributes* params);
    void draw();
    void reload();
    
private:
    void createParticle();
    void updateParticles();
    
    ofFbo fbo;
    ofShader shader;
    float decay;
    
    vector<Particle> particles;
    vector<ofImage> textures;
    ofTexture currentTexture;
    float spriteSize;
    
    ofVboMesh billboards;
    
};

