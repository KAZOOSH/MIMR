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

class Universum: public Layer{
    
public:
    
    void setup(RadarAttributes* params);
    void draw();
    void reload();
    
private:
    ofFbo fbo;
    ofImage img;
    ofShader shader;
    
};

