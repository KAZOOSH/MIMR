#version 150

uniform sampler2DRect wave;

uniform int width;
uniform int height;
uniform int shaper;

uniform float time;

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

vec4 shapeLinear(){
	return texture(wave, gl_FragCoord.xy);
}

vec4 shapeSin(){
	//lreturn texture(wave, gl_FragCoord.xy);
	return texture(wave, gl_FragCoord.xy+ sin(gl_FragCoord.y/40)*20);// + vec2 (gl_FragCoord.x+ sin(gl_FragCoord.y/40), 0));
}

vec4 shapeTriangle(){
	float dx = mod(gl_FragCoord.y,80);
	if(dx>40) dx = 80-dx;
	return texture(wave, gl_FragCoord.xy+ vec2(dx,0));
}

vec4 shapeQuad(){
	float dx = mod(int(gl_FragCoord.y/40),2)*30;
	float x = mod(gl_FragCoord.x-dx,width);
	return texture(wave, vec2(x,gl_FragCoord.y));
}

void main()
{
	//outputColor =shapeTriangle();

	if (shaper == 0){
		outputColor = shapeLinear();
	}else if(shaper == 1){
		outputColor = shapeSin();
	}else if(shaper == 2){
		outputColor = shapeTriangle();
	}else if(shaper == 3){
		outputColor = shapeQuad();
	}else{
		outputColor = vec4(0.0,0.0,0.0,0.0);
	}
	
    //outputColor = vec4(dist,dist,dist,1.0);
    //outputColor = vec4(value,value,value,1.0);
}