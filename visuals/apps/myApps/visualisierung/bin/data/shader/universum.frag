#version 150

#define M_PI 3.1415926535897932384626433832795
#define R_FADEOUT 12.0

// this is how we receive the texture
uniform sampler2DRect tex0;

uniform float angle;
uniform float time;

uniform float spiral; // -0.01 to 0.4
uniform float detail; // 0.0 to 1.0   0.659
uniform float center; //0.0 to 1.0  0.9
uniform float centerBlur; //0.0 - 1.0

uniform int colorSet;

in vec2 texCoordVarying;

out vec4 outputColor;

float cosAngle(vec2 v1, vec2 v2){
    return dot(v1, v2) / (length(v1) * length(v2));
}

float map(float inV, float inMin, float inMax, float outMin, float outMax){
    float w1 = inMax - inMin;
    float w2 = outMax - outMin;
    float w = w1/w2;
    return clamp((inV-inMin)*w+outMin,outMin,outMax);

}


void main()
{
    vec2 iResolution = vec2(1024,1024);

    vec2 uv = (texCoordVarying.xy/iResolution.xy)-.5;
    float len = length(uv);                             //coords
    
    float t = .001*time;
    float time2 = t  +  (5.+sin(t))*.11 / (len+spiral); // spiraling
    float si = sin(time2), co = cos(time2);
    uv *= mat2(co, si, -si, co);                    // rotation

    float c=0., v1=0., v2=0., glow;  vec3 p;
    
    for (int i = 0; i < 90; i++) {                  //creating pattern
        p = .035*float(i) *  vec3(uv, 1.);
        p += vec3(.22,  .3,  -1.5 -sin(t*1.3)*.1);
        
        for (int i = 0; i < 7; i++)                // IFS
            p = abs(p) / dot(p,p) - detail;//0.659;

        float p2 = dot(p,p)*.0015;
        v1 += p2 * ( 1.8 + sin(len*13.0  +.5 -t*2.) );
        v2 += p2 * ( 1.5 + sin(len*13.5 +2.2 -t*3.) );
    }
    
    c = length(p.xy) * .175;
    v1 *= smoothstep(.7 , .0, len);
    v2 *= smoothstep(.6 , .0, len);

    //calculate glow
    glow  = smoothstep(center*.08 +(centerBlur* .06 +.04),center*.08 +(1-centerBlur)*.025, len);
    
    //fade out behind radar
    float distance = length(vec2(512,512) - texCoordVarying);
    float x = cos(angle*M_PI/180 + M_PI*0.7);
    float y = sin(angle*M_PI/180 + M_PI*0.7);

    float angle = cosAngle(vec2(x,y),vec2(512,512) -texCoordVarying);
    angle = clamp(angle,0.0,1.0);
    angle *= angle;
    //angle *= angle;
    angle *= 0.3;

    //circular ease out
    float fadeOut = 0;
    if (distance > 512){
        fadeOut = 1;
    }
    else if (distance > 512 - R_FADEOUT)
    {
        float d =(distance-512+ R_FADEOUT)/R_FADEOUT;
        d = clamp(d,0.0,1.0);
        //fadeOut = sqrt(1 - d*d);
        
        fadeOut = 1-sqrt (1 - d*d);
    }

    vec3 col;

    if (colorSet == 0){ //normal
    	col = vec3(c,  (v1+c)*.25,  v2);
    }
    else if (colorSet == 1){ //dark
    	col = vec3((v1+c)*.4,  (v1+c)*.4,  (v1+c)*.4);
    }
    else if (colorSet == 2){ //green
    	col = vec3(v2*.95,(v1+c)*.25,    (v1+c)*.35);
    }
    else if (colorSet == 3){ //pink
    	col = vec3((v1+c)*.3,  v2*.7,  (v1+c)*.5);
    }

    //mix final color
    //vec3 col = vec3(c,  (v1+c)*.25,  v2);
    //col = vec3((v1+c)*.25,  (v1+c)*.25,  (v1+c)*.25);
    //col = vec3(v2,v2,v2);
    float alphaCenter = map (center,0.0,0.5,0.0,0.7);
    col = col  +  glow*alphaCenter;// -angle;  

    outputColor=vec4(col, 1. - fadeOut);

}