#ifndef _GeneralPurposeFunctions
#define _GeneralPurposeFunctions

#include "ofMain.h"

namespace ofxModule {

	/// \brief as the name indicates, a class with static useful functions
	class GeneralPurposeFunctions {

	public:

	public:
		/// \brief converts a UNIX timestampstring to a string of format "%y-%m-%dT%H:%M:%S+0000"
		static string UnixToIsoTimeStamp(string parameter);

		static ofColor colorFromJson(ofJson val);
		static ofVec2f vec2fFromJson(ofJson val);
		static ofVec3f vec3fFromJson(ofJson val);

	protected:

	private:

	};

}

#endif
