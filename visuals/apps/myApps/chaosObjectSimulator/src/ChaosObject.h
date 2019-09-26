#pragma once
#include "ofMain.h"

class ChaosObject
{
public:
	ChaosObject();
	~ChaosObject();
	
	void drawDebug();
	ofVec2f getPosition();

	int id;
	int channel;
	int note;
	bool isActive = false;
	bool hasSend = false;

	float angle; //0-1
	float radius; //0-1

};

