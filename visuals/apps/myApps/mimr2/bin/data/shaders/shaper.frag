#version 150

uniform sampler2DRect wave;

uniform int width;
uniform int height;
uniform int shaper0;
uniform int shaper1;
uniform int shaper2;

uniform float time;
uniform float intensity;

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
	float nShapes = 14;
	float wShape = float(width)/nShapes/3.14;  
	float dx = sin(gl_FragCoord.y/wShape)*40*intensity;
	if(gl_FragCoord.x + dx > width) dx -= width;
	if(gl_FragCoord.x + dx < 0) dx += width;

	vec4[5] colors;

	 colors[0] = texture(wave, gl_FragCoord.xy+ vec2(dx,0));
	 colors[1] = texture(wave, gl_FragCoord.xy+ vec2(dx+1,0));
	 colors[2] = texture(wave, gl_FragCoord.xy+ vec2(dx-1,0));
	 colors[3] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));
	 colors[4] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));

	vec4 vout = vec4(0,0,0,1);
	for(int i=0;i<5;++i){
		if(vout.r<colors[i].r) vout = colors[i];
	}

	return vout;
}

vec4 shapeSin(){
	float nShapes = 3;
	float wShape = float(width)/nShapes;  
	float dx = mod(gl_FragCoord.y,wShape);
	if(dx>wShape/2) dx = wShape-dx;
	dx *= intensity*1.2;
	if(gl_FragCoord.x + dx > width) dx -= width;
	vec4[5] colors;

	 colors[0] = texture(wave, gl_FragCoord.xy+ vec2(dx,0));
	 colors[1] = texture(wave, gl_FragCoord.xy+ vec2(dx+1,0));
	 colors[2] = texture(wave, gl_FragCoord.xy+ vec2(dx-1,0));
	 colors[3] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));
	 colors[4] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));

	vec4 vout = vec4(0,0,0,1);
	for(int i=0;i<5;++i){
		if(vout.r<colors[i].r) vout = colors[i];
	}

	return vout;
}

vec4 shapeTriangle(){
	float nShapes = 18;
	float wShape = float(width)/nShapes;  
	float dx = mod(gl_FragCoord.y,wShape);
	if(dx>wShape/2) dx = wShape-dx;
	dx *= intensity*1.2;
	if(gl_FragCoord.x + dx > width) dx -= width;
	
	vec4[5] colors;

	 colors[0] = texture(wave, gl_FragCoord.xy+ vec2(dx,0));
	 colors[1] = texture(wave, gl_FragCoord.xy+ vec2(dx+1,0));
	 colors[2] = texture(wave, gl_FragCoord.xy+ vec2(dx-1,0));
	 colors[3] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));
	 colors[4] = texture(wave, gl_FragCoord.xy+ vec2(dx-2,0));

	vec4 vout = vec4(0,0,0,1);
	for(int i=0;i<5;++i){
		if(vout.r<colors[i].r) vout = colors[i];
	}

	return vout;
}

vec4 shapeQuad(){
	float dx = mod(int(gl_FragCoord.y/40),2)*intensity*40;
	float x = mod(gl_FragCoord.x-dx,width);
	return texture(wave, vec2(x,gl_FragCoord.y));

	vec4[5] colors;

	 colors[0] = texture(wave, vec2(x,gl_FragCoord.y));
	 colors[1] = texture(wave, vec2(x-0.5,gl_FragCoord.y));
	 colors[2] = texture(wave, vec2(x-1,gl_FragCoord.y));
	 colors[3] = texture(wave, vec2(x+1,gl_FragCoord.y));
	 colors[4] = texture(wave, vec2(x+0.5,gl_FragCoord.y));

	vec4 vout = vec4(0,0,0,1);
	for(int i=0;i<5;++i){
		if(vout.r<colors[i].r) vout = colors[i];
	}

	return vout;
}

void main()
{
	//outputColor =shapeTriangle();

	vec4 greyOut;


	if(shaper0 == 1){
		greyOut = shapeSin();
	}else if(shaper1 == 1){
		greyOut = shapeTriangle();
	}else if(shaper2 == 1){
		greyOut = shapeQuad();
	}else{
		greyOut = shapeLinear();
	}
    outputColor = greyOut;
}