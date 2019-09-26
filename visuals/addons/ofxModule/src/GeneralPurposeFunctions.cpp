#include "GeneralPurposeFunctions.h"

namespace ofxModule {

	string GeneralPurposeFunctions::UnixToIsoTimeStamp(string parameter) {
		char newTime[300];
		int intTime = atoi(parameter.c_str());
		time_t rawTime = intTime;
		struct tm *timeinfo = localtime(&rawTime);
		strftime(newTime, 300, "%y-%m-%dT%H:%M:%S+0000", timeinfo);
		std::string time = newTime;
		return newTime;
	}

	ofColor GeneralPurposeFunctions::colorFromJson(ofJson val)
	{
		switch (val.size())
		{
		case 1:
			// interprete string
			if (val.is_string()) {
				
				if (ofIsStringInString(val.get<string>(), ".")) {
					ofParameter<ofFloatColor> pTemp;
					pTemp.fromString(val);
					return pTemp.get();
				}
				else {
					ofParameter<ofColor> pTemp;
					pTemp.fromString(val);
					return pTemp.get();
				}
				
			}// interprete array
			else
				return ofColor(val[0].get<int>());

			break;
		case 2:
			return ofColor(val[0], val[0], val[0], val[1]);
			break;
		case 3:
			return ofColor(val[0], val[1], val[2]);
			break;
		case 4:
			return ofColor(val[0], val[1], val[2], val[3]);
			break;
		default:
			return ofColor();
			break;
		}

	}

	ofVec2f GeneralPurposeFunctions::vec2fFromJson(ofJson val)
	{
		switch (val.size())
		{
		case 1:
			// interprete string
			if (val.is_string()) {
				ofParameter<ofVec2f> pTemp;
				pTemp.fromString(val);
				return pTemp.get();
			}// interprete array
			else
				return ofVec2f(val[0].get<int>());

			break;
		case 2:
			return ofVec2f(val[0], val[1]);
			break;
		default:
			return ofVec2f();
			break;
		}
	}

	ofVec3f GeneralPurposeFunctions::vec3fFromJson(ofJson val)
	{
		switch (val.size())
		{
		case 1:
			// interprete string
			if (val.is_string()) {
				ofParameter<ofVec3f> pTemp;
				pTemp.fromString(val);
				return pTemp.get();
			}// interprete array
			else
				return ofVec3f(val[0].get<int>());

			break;
		case 2:
			return ofVec3f(val[0], val[1]);
			break;
		case 3:
			return ofVec3f(val[0], val[1], val[2]);
			break;
		default:
			return ofVec2f();
			break;
		}
	}

}