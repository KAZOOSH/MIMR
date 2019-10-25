//
//  GoldenBox.h
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//  Copyright (c) 2017 . All rights reserved.
//

#ifndef _4_simpleTexturing_GoldenBox_cpp
#define _4_simpleTexturing_GoldenBox_cpp

#include "ofMain.h"
#include "Layer.h"

class GoldenBox: public Layer{
    
public:
    
    void setup(RadarAttributes* params);
    void draw();
    
private:
    ofFbo fbo;
    
    float rMin;
    float rMax;
};


#endif