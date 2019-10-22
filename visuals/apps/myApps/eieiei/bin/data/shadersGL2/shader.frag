//hintergrund dynamische farbe
//bälle das gleiche
//https://color.adobe.com/de/Psychedelic-Pop-color-theme-1229859/edit/?copy=true


#version 120

#define AA 4.

#define PURPLE vec3(0.64,0,0.69)
#define PINK vec3(1,0,0.59)
#define TURQUOISE vec3(0,0.73,0.83) 
#define BLUE vec3(0,0.38,0.68) 

// this is how we receive the texture
uniform sampler2DRect tex0;
uniform float iTime;
uniform float depth;

varying vec2 tcoord;

//---------------Improved RGB--------------

/*
	The idea behind this function is to avoid the low saturation area in the
	rgb color space. This is done by getting the direction to that diagonal
	and displacing the interpolated	color by it's inverse while scaling it
	by saturation error and desired lightness. 

	I find it behaves very well under most circumstances, the only instance
	where it doesn't behave ideally is when the hues are very close	to 180 
	degrees apart, since the method I am using to find the displacement vector
	does not compensate for non-curving motion. I tried a few things to 
	circumvent this problem but none were cheap and effective enough..
*/

//Changes the strength of the displacement
#define DSP_STR 1.5


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

	if (x < 0.25){
		return iLerp(PURPLE, PINK, x*4.0);
	}
	else if (x < 0.5){
		return iLerp(PINK, TURQUOISE, (x-0.25)*4.0);
	}
	else if (x < 0.75){
		return iLerp(TURQUOISE, BLUE, (x-0.5)*4.0);
	}
	else{
		return iLerp(BLUE, PURPLE, (x-0.75)*4.0);
	}
}


float metaball(vec2 p, float r)
{
	return r / dot(p, p);
}

vec4 samplef(in vec2 uv)
{
	float t0x = -0.2 + cos(iTime) * .3;
	float t0y = -0.2 + cos(iTime * 2.4) * .3;
	float t1x = -0.5 + sin(iTime * .5) * 0.25;
	float t1y = -0.5 + sin(iTime * .3) * 0.35;
	float t2x = -0.1 + cos(iTime * 0.1) * 0.7; //große kurve
	float t2y = - 0.1 +sin(iTime * 0.05) * 0.7;
	float t3x = 0.6 + cos(iTime * 1.5) * 0.33; 
	float t3y = 0.2 +sin(iTime * 2.0) * 0.3;
	float t4x = 0.5 + cos(iTime * 0.8) * 0.2; 
	float t4y = 0.65 +sin(iTime * 0.6) * 0.2;
	float t6y = -0.49 +cos(iTime * 0.2) * 0.4;
	float t5x = 0.65 + cos(iTime * 0.7) * 0.3; //außerhalb
	float t5y = -0.65 +sin(iTime * 0.3) * 0.3;



	float r = metaball(uv + vec2(t0x, t0y), .02 ) * //unten rechts
			  metaball(uv + vec2(t1x, t1y), .07)+

			  metaball(uv + vec2(t3x, t3y), .08) * // oben links
			  metaball(uv + vec2(t4x, t4y), .05) +

			  metaball(uv + vec2(t0x, t4x), .07) * // oben rechts
			  metaball(uv + vec2(t1y, t4y), .09) +

			  metaball(uv + vec2(t3y, t0x), .04) * //unten links
			  metaball(uv + vec2(t3x, t1y), .08) +
			  metaball(uv + vec2(t3x, t6y), .03) +

			  metaball(uv + vec2(t5y, t5y), .06); +

			  metaball(uv + vec2(t5x, t5x), .05) +

			  metaball(uv + vec2(t2x, t2y), .04);

	
    

	vec3 bg =  vLerp( cos(depth));

	vec3 col = vLerp(sin(depth));


	vec3 c = bg;

	float alpha = 0;
	//if (r>.5) c = r/2 * CE;

	if (r>.3) c = 0.4 * col;
	if (r>.4) c = bg;
	if (r>.43) c = 0.6 * col;
	if (r>.53) c = bg;
	if (r>.56) c = 0.8 * col;
	if (r>.66) c = bg;
	if (r>.69) c = col;
	if (r>.9) c = bg;

	if (c!=bg) alpha = 1;
	return vec4(c,1);
}

void main()
{
	vec2 fragCoord = tcoord;
	vec2 iResolution = vec2(800,600);

	vec2 uv = (fragCoord.xy / iResolution.xy * 2. - 1.)
			* vec2(iResolution.x / iResolution.y, 1) * 1.25;

    vec4 col = vec4(0);

    // Antialiasing via supersampling
    float e = 1. / min(iResolution.y , iResolution.x);    
    for (float i = -AA; i < AA; ++i) {
        for (float j = -AA; j < AA; ++j) {
    		col += samplef(uv + vec2(i, j) * (e/AA)) / (4.*AA*AA);
        }
    }
    
    //fragColor = vec4(clamp(col, 0., 1.), 1)
 	//gl_FragColor = vec4(uv.x,uv.y,0,1);
    gl_FragColor = vec4(clamp(col, 0., 1.));
}