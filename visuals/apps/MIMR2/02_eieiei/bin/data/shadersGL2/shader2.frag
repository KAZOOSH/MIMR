            #define AA 4.

#define PURPLE vec3(0.64,0,0.69)
#define PINK vec3(1,0,0.59)
#define TURQUOISE vec3(0,0.73,0.83) 
#define BLUE vec3(0,0.38,0.68) 

    uniform vec4 color;
    uniform vec2 scale;
    uniform vec2 offset;
    varying vec2 tcoord;

uniform int colorSet;
uniform float iTime;
uniform float depth;
uniform float isIdle;

#define DSP_STR 1.5
#define PI_HALF 1.57079632679

vec3 color1,color2,color3,color4;

float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}

//Improved rgb lerp
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    //Interpolated base color (with singularity fix)
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    
    //Saturation difference from ideal scenario
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    
    //Displacement direction
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    //Simple Lighntess
    float lgt = dot(vec3(1.0), ic);
    
    //Extra scaling factor for the displacement
    float ff = dot(dir, normalize(ic));
    
    //Displace the color
    ic += DSP_STR*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}
vec3 vLerp(float x){
    x = abs(x);

    

    if(colorSet == 0){
        color1 = PURPLE;
        color2 = PINK;
        color3 = TURQUOISE;
        color4 = BLUE;
    }
    else if(colorSet == 1){
        color1 = vec3(0.09,1.0,0.0);
        color2 = vec3(0.2,1.0,0.0);
        color3 = vec3(0.3,0.3,0.3);
        color4 = vec3(0.1,0.1,0.1);
    }
    else if(colorSet == 2){
        color1 = vec3(.95,.05,.01);
        color2 = vec3(.7,.1,.15);
        color3 = vec3(.1,.25,.4);
        color4 = vec3(0.1,0.12,0.17);
    }
    else if(colorSet == 3){
        color1 = vec3(.09,.5,.17);
        color2 = vec3(.3,.65,.69);
        color3 = vec3(.4,.27,.6);
        color4 = vec3(.2,.2,.5);
    }


    if (x < 0.25){
        return iLerp(color1, color2, x*4.0);
    }
    else if (x < 0.5){
        return iLerp(color2, color3, (x-0.25)*4.0);
    }
    else if (x < 0.75){
        return iLerp(color3, color4, (x-0.5)*4.0);
    }
    else{
        return iLerp(color4, color1, (x-0.75)*4.0);
    }
}


float metaball(vec2 p, float r)
{
    return r / dot(p, p);
}

vec3 samplef(in vec2 uv)
{
    float t0x = -0.2 + cos(iTime) * .3;
    float t0y = -0.2 + cos(iTime * 2.4) * .3;
    float t1x = -0.7 + sin(iTime * .5) * 0.25;
    float t1y = -0.7 + sin(iTime * .3) * 0.35;
    //float t2x = -0.1 + cos(iTime * 0.1) * 0.9; //grosse kurve
    //float t2y = - 0.1 +sin(iTime * 0.05) * 0.88;
    float t3x = 0.8 + cos(iTime * 1.5) * 0.33; 
    float t3y = 0.2 +sin(iTime * 2.0) * 0.3;
    float t4x = 0.5 + cos(iTime * 0.8) * 0.2; 
    float t4y = 0.7 +sin(iTime * 0.6) * 0.2;
    //float t6y = -0.6 +cos(iTime * 0.2) * 0.5;
    //float t5x = 1.0 + cos(iTime * 0.7) * 0.3; //ausserhalb
    //float t5y = -1.0 +sin(iTime * 0.3) * 0.3;

    //float d = 1.0-(depth/127.0) +1.0;
    float d = depth/127.0 +1.0;

    float r = metaball(uv + vec2(t0x, t0y), .02 *d) * //unten rechts
              metaball(uv + vec2(t1x, t1y), .07*d)+

              metaball(uv + vec2(t3x, t3y), .1*d) * // oben links
              metaball(uv + vec2(t4x, t4y), .05*d)+

              metaball(uv + vec2(t0x, t4x), .07*d) * // oben rechts
              metaball(uv + vec2(t1y, t4y), .12*d) +

              metaball(uv + vec2(t3y, t0x), .04*d) * //unten links
              metaball(uv + vec2(t3x, t1y), .09*d) +
              metaball(uv + vec2(t3x, t4y), .03*d);// +

              //metaball(uv + vec2(t5y, t5y), .06*d) +

              //metaball(uv + vec2(t5x, t5x), .05*d) +

              //metaball(uv + vec2(t2x, t2y), .04*d);

    
    

    //vec3 bg = vLerp( cos(iTime/10.0));
    //vec3 bg = vLerp( cos(depth*PI_HALF));
    vec3 bg = vLerp( depth*0.9/127.0);

    vec3 col;

    if(depth < 30.0) col = color3;
    else if(depth < 60.0) col = color4;
    else if(depth < 90.0) col = color1;
    else col = color2;

    vec3 c = bg;

    

    if (r>.3) c = 0.4 * col;
    if (r>.4) c = bg;
    if (r>.43) c = 0.6 * col;
    if (r>.53) c = bg;
    if (r>.56) c = 0.8 * col;
    if (r>.66) c = bg;
    if (r>.69) c = col;
    if (r>.9) c = bg;

    return c;
}

    uniform sampler2D tex;
    void main(void) {
    vec2 fragCoord = vec2(gl_FragCoord.x,gl_FragCoord.y);
    vec2 iResolution = vec2(480.0,480.0);

    if(fragCoord.x > iResolution.x || fragCoord.y > iResolution.y ){
        gl_FragColor = vec4(0.0,0.0,0.0, 1.0);
    }else{

    vec2 uv = (fragCoord.xy / iResolution.xy * 2. - 1.)
            * vec2(iResolution.x / iResolution.y, 1) * 1.25;

    vec3 col = vec3(uv.x,uv.y,0.0);


    col = samplef(uv);  
    
    if(isIdle == 1.0) col*=0.3;
    else col -= isIdle*0.2;

    col = clamp(col,0.0,1.0);
    
    gl_FragColor = vec4(col, 1.0);}
    }