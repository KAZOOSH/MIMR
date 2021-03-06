#version 150

#define M_PI 3.1415926535897932384626433832795

// this is how we receive the texture
uniform sampler2DRect tex0;
//uniform sampler2DRect tex1;

uniform float speed;
uniform float angle;
uniform float time;
uniform int colorSet;

in vec2 texCoordVarying;

out vec4 outputColor;

float radius[3];// = {500,480,460,440,420};

float widthLine = 4;
 


vec4 mod289(vec4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
 
vec4 permute(vec4 x)
{
    return mod289(((x*34.0)+1.0)*x);
}
 
vec4 taylorInvSqrt(vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}
 
vec2 fade(vec2 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}
 
// Classic Perlin noise
float cnoise(vec2 P)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod289(Pi); // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
     
    vec4 i = permute(permute(ix) + iy);
     
    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
    vec4 gy = abs(gx) - 0.5 ;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
     
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
     
    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;  
    g01 *= norm.y;  
    g10 *= norm.z;  
    g11 *= norm.w;  
     
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
     
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}
 
// Classic Perlin noise, periodic variant
float pnoise(vec2 P, vec2 rep)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
    Pi = mod289(Pi);        // To avoid truncation effects in permutation
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
     
    vec4 i = permute(permute(ix) + iy);
     
    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
    vec4 gy = abs(gx) - 0.5 ;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
     
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
     
    vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
    g00 *= norm.x;  
    g01 *= norm.y;  
    g10 *= norm.z;  
    g11 *= norm.w;  
     
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
     
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy;
}
 
float fbm(vec2 P, int octaves, float lacunarity, float gain)
{
    float sum = 0.0;
    float amp = 1.0;
    vec2 pp = P;
     
    int i;
     
    for(i = 0; i < octaves; i+=1)
    {
        amp *= gain; 
        sum += amp * cnoise(pp);
        pp *= lacunarity;
    }
    return sum;
 
}

float noise(float cosAngle, float multiplier){
    return fbm(vec2(cosAngle,(time*multiplier)/100), 2,6,0.4);
}

float cosAngle(vec2 v1, vec2 v2){
    return dot(v1, v2) / (length(v1) * length(v2));
}

void main()
{
	float distance = length(vec2(512,512) - texCoordVarying);
	//float alpha = (texCoordVarying.x - 512)/distance;


    //calculate angle
    float cosAlpha = cosAngle(texCoordVarying - vec2(512,512), vec2(1,0));
    float alpha = acos(cosAlpha);

    alpha/=M_PI;
	if (texCoordVarying.y >= 512) alpha = (1-alpha)*0.5 +0.5;
    else alpha *= 0.5;

    int iTex = 4096 - int(alpha * 4096);
    vec2 pTex = vec2(iTex%1024, iTex/1024*3+1);


//distance makes freaky effect
    //radius[0] = 500 + fbm(vec2(cosAlpha,(distance+ angle)/100), 2,6,0.4)*50;
    float dFlows = texture(tex0, pTex).r ;

    radius[0] = 490 + noise(cosAlpha,1.0)*50;
    radius[1] = 490 + noise(cosAlpha,1.0 + dFlows*0.2)*50;
    radius[2] = 490 + noise(cosAlpha,1.0 + dFlows*0.3)*50;


	vec4 color = vec4(0,0,0,0);

    //if (t>5 && t<5.01) color = vec4(0,0,0,1);


    float angleNorm = 1 -angle/360;
    float decay = angleNorm - alpha;
    if (angleNorm < alpha) decay = angleNorm + 1 -alpha;
    decay = (1 - decay)*1.2;


    float brightness = max(texture(tex0, pTex).r  -decay,0);

    vec3 ccolor;
    if(colorSet == 0) ccolor = vec3(0.04,0.2,1.0);
    else if(colorSet == 1) ccolor = vec3(0.5,1,0.12);
    else if(colorSet == 2) ccolor = vec3(0.7,0,0.2);
    else if(colorSet == 3) ccolor = vec3(0.1,.9,0.8);

    float b = 0;
    for(int i=0; i<3; ++i){
        if(distance >= radius[i] && distance <= radius[i] + widthLine){
            color = vec4(ccolor,brightness);
            //b+= brightness;
            
        }
        //color = vec4(1,1,1,b);
    }
    

    //color = texture(tex0, vec2(texCoordVarying.x,1024-texCoordVarying.y));

    //outputColor = vec4(alpha2/3,-alpha2/3,0,1);
    //color.a = min(color.a,1.0);

    //outputColor = vec4(decay,0,0,1.0);
    outputColor = color;
    //outputColor = texture(tex0, pTex);
}