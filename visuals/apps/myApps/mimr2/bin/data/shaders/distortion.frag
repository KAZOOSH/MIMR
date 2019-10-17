#version 150

uniform sampler2DRect tex;

uniform int width;
uniform int height;

uniform float time;
uniform float intensity;

uniform float radius;
uniform vec2 center;

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

float mapE(float value, float shaper){
	//pct = pow(pct, shaper);
	//return map(pct, 0,1, outMin, outMax);
    return pow(value, shaper);
}


vec2 remapCoords(vec2 coords){
    float d = distance(vec2(0,0),coords);

    //float dx = 1 + mapN(d,0,1,0,1,intensity) - d;
    float strength = map(intensity,0,1,0.3,4);
    float dx = 1 + mapE(d,strength) - d;

    vec2 outProjectedCoords =  vec2(coords.x*dx,coords.y*dx );

    return  outProjectedCoords;
}

void main()
{
    float dist = distance(gl_FragCoord.xy,center);

    if  (dist < radius){
        vec2 uv = gl_FragCoord.xy/vec2(width,height);
        uv -= vec2(0.5,0.5);
        uv*=2;

        vec2 cellDelta = remapCoords(uv);
        vec2 xy = (cellDelta*0.5) + vec2(0.5,0.5);
        vec4 col = texture(tex, xy*vec2(width,height));
        
        outputColor = col;
    }else{
        outputColor = texture(tex, gl_FragCoord.xy);
    }

	
}