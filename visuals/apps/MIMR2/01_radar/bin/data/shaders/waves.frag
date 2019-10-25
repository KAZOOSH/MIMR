#version 150

#define PI 3.145926
#define DlineWidth 1

uniform sampler2DRect wave;
uniform sampler2DRect shaper;

uniform int width;
uniform int height;
uniform vec2 center;
uniform float time;
uniform float radius;

uniform vec2 centerShaper;
uniform float rShaper;
uniform vec2 centerLineWidth;
uniform float rLineWidth;
uniform int lineWidth;

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

	// get distance from center of texture
	float dist = distance(gl_FragCoord.xy,center);
	int currentTex = 0;

	// when shaper is growing set shaper texture as texture if in radius
	float dShaper = distance(gl_FragCoord.xy,centerShaper);
	if(dShaper < rShaper){
		currentTex = 1;
	}

	int tLineWidth = DlineWidth;
	if(distance(gl_FragCoord.xy,centerLineWidth) < rLineWidth){
		tLineWidth = lineWidth;
	}


	// trigonometry for angle setting 
	float cosAlpha = cosAngle(gl_FragCoord.xy - center, vec2(1,0));
    float alpha = acos(cosAlpha);
    if(gl_FragCoord.y<center.y){
    	alpha = 2*PI - alpha;
    }

    


    // draw waves if pixel is in wave radius
	if(dist < radius){
		
		dist = radius -dist;
		float pos = time + dist;
		if(pos > radius) pos -= radius;
		vec4 tcol = texture(wave, vec2(pos,height*0.5/PI * alpha));

		// thicken the line 
		
		if(currentTex == 1){
			tcol = texture(shaper, vec2(pos,height*0.5/PI * alpha));
			for(int i=1; i<tLineWidth;++i){
				float pt = pos - i;
				if(pt<0) pt = width-1;
				vec4 c = texture(shaper, vec2(pt,height*0.5/PI * alpha));
				if (c.r > tcol.r){
					tcol = c;
				}
			}
		}else{
			for(int i=1; i<tLineWidth;++i){
				float pt = pos - i;
				if(pt<0) pt = width-1;
				vec4 c = texture(wave, vec2(pt,height*0.5/PI * alpha));
				if (c.r > tcol.r){
					tcol = c;
				}
			}
		}

		float dColor = map(dist,radius-500,radius-100,0,1,true);
		//tcol *= vec4(1.0,1.0*dColor,1.0*dColor,1.0);
		if(tcol.r == 1.0) tcol.a = 1.0;
		else tcol.a = 0.0;
		outputColor = tcol;

		//outputColor = vec4(alpha/2/PI,0,0,1.0);
	}else{
		outputColor = vec4(0.0,0.0,0.0,0.0);
	}
    //outputColor = vec4(dist,dist,dist,1.0);
    //outputColor = vec4(value,value,value,1.0);
}