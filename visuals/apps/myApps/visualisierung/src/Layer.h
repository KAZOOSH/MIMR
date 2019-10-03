//
//  Layer.h
//  04_simpleTexturing
//
//  Created by Brian Eschrich on 20.09.17.
//
//

#ifndef Layer_h
#define Layer_h

#include "ofMain.h"

class RadarAttributes{
public:
    float toRad(float angle){
        return angle*PI/180;
    }
    
    float angle; //in °
    float lastAngle; //in °
    float time;
    float lastTime;
    
    float bpm;
    float lastTick;
    
    ofParameter<int> colorSet;
    //  values for objects
    ofParameter<int> v_kurbel;
    ofParameter<bool> v_kuehler1;
    ofParameter<bool> v_kuehler2;
    ofParameter<bool> v_kuehler3;
    ofParameter<bool> v_kuehler4;
    ofParameter<bool> v_kuehler5;
    ofParameter<int> v_theremin;
    ofParameter<int> v_trichter_rot;
    ofParameter<int> v_trichter_mic;
    ofParameter<int> v_eieiei_spin;
    ofParameter<int> v_eieiei_membrane;
    ofParameter<int> v_goldenBox;
    ofParameter<int> v_bassfahrer;
    ofParameter<int> v_foehn;
    ofParameter<bool> v_foehn_schalter1;
    ofParameter<bool> v_foehn_schalter2;
    ofParameter<bool> v_foehn_schalter3;
	ofParameter<bool> v_bank_schalter1;
	ofParameter<bool> v_bank_schalter2;
	ofParameter<bool> v_bank_schalter3;

    
};

class Layer{
public:
    
    virtual void setup(RadarAttributes* params);
    virtual void draw();
    virtual void reload();
    
    static float map(float in, float inMin, float inMax, float outMin, float outMax, float shaper);
    
protected:
    RadarAttributes* params;
    
    
};


#endif /* Layer_hpp */
