//
//  Kurbel.h
//  visualisierung
//
//  Created by Brian Eschrich on 10.10.17.
//
//

#ifndef Kurbel_h
#define Kurbel_h

#define STEPSIZE_TEX 11.3777777778

#include "ofMain.h"
#include "Layer.h"

class Kurbel: public Layer{
    
public:
    
    void setup(RadarAttributes* params);
    void draw();
    void reload();
    
private:
    ofFbo valueFbo;
    ofFbo fbo;
    ofShader shader;
    
    float radius;
    float distance;
    
    ofImage img;
};

#endif /* Kurbel_h */
