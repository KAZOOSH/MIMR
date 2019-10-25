#version 150

#define PI 3.145926

uniform sampler2DRect tex;

uniform int width;
uniform int height;
uniform vec2 center;
uniform float time;
uniform float radius;
uniform float maxAbberation;

out vec4 outputColor;


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

	float dist = distance(gl_FragCoord.xy,center);
	vec2 vD = gl_FragCoord.xy - center;

	if(dist < radius){
		float abbStrength = map(dist,radius-600,radius-50,0,1,true)*maxAbberation;
		vec2 dAbberation = normalize(vD)*abbStrength;

		float r = texture(tex, gl_FragCoord.xy + dAbberation).r;
		float g = texture(tex, gl_FragCoord.xy).g;
		float b = texture(tex, gl_FragCoord.xy - dAbberation).b;

		outputColor = vec4(r,g,b,texture(tex, gl_FragCoord.xy).a);
	}else{
		outputColor = texture(tex, gl_FragCoord.xy);
	}
}