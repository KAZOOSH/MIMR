#include "ChaosObject.h"



ChaosObject::ChaosObject()
{
}


ChaosObject::~ChaosObject()
{
}

void ChaosObject::drawDebug()
{
		isActive ? ofSetColor(0, 255, 0) : ofSetColor(255, 0, 0);
		ofDrawBitmapString(ofToString(id), getPosition() + ofVec2f(5, 0));
		ofDrawCircle(getPosition(), 2);
}

ofVec2f ChaosObject::getPosition()
{
	int w = ofGetHeight();
	ofVec2f center = ofVec2f(ofGetWidth(), ofGetHeight()) / 2;
	return center + ofVec2f(cos(angle* 2 * PI)*radius, sin(angle * 2 * PI)*radius);
}
