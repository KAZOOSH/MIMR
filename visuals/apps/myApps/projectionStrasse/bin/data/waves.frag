#version 150

#define PI 3.145926
#define DlineWidth 1

uniform sampler2DRect tex1;

uniform int width;
uniform int height;
uniform vec2 center;
uniform float time;

out vec4 outputColor;

#define FLAMINGO vec3(1.0,0.65,0.71)
#define TURQUOISE vec3(0,0.73,0.83) 
#define BLUE vec3(0,0.38,0.68) 


float distance(vec2 v1,vec2 v2){
	return length(v2-v1);
}

float distance(float v1,float v2){
	return abs(v2-v1);
}

float angle(vec2 v1, vec2 v2){
	return 12.0;
}

float cosAngle(vec2 v1, vec2 v2){
	return dot(v1,v2)/(length(v1)*length(v2));
}

float map(float value, float min1,float max1,float min2, float max2,bool clamp){
	if (clamp && value <= min1 ) return min2;
	else if (clamp && value >= max1 ) return max2;
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float map(float value, float min1,float max1,float min2, float max2){
	return map(value,  min1, max1, min2, max2,false);
}

float mapN(float value, float inMin, float inMax, float outMin, float outMax, float shaper){
	float pct = map (value, inMin, inMax, 0, 1);

	pct = pow(pct, shaper);
	return map(pct, 0,1, outMin, outMax);
}

void main()
{
	float wLine = 20;
	float tLine = 2;
	// get distance from center of texture
	float dist = distance(gl_FragCoord.xy,center);

	float col = mod(dist,wLine*2);

	float offset = mod(time,tLine);
	offset = wLine*2 - map(offset,0,tLine,0,wLine*2);
	col += offset;
	if (col > wLine*2) col -= wLine*2;

	if(col > wLine)
		outputColor = vec4(TURQUOISE.rgb,1.0);
	else{
		outputColor = vec4(0,0,0,0);
	}
	
    //outputColor = vec4(dist,dist,dist,1.0);
    //outputColor = vec4(value,value,value,1.0);
}