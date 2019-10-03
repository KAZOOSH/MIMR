#version 150

uniform int width;
uniform int height;
uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform sampler2DRect tex2;
uniform sampler2DRect tex3;
uniform sampler2DRect tex4;
uniform sampler2DRect tex5;
uniform sampler2DRect tex6;
uniform sampler2DRect tex7;
uniform float time;

out vec4 outputColor;

int nTex = 8;
vec4[8] colorValues; 

float distance(float v1,float v2){
	return abs(v2-v1);
}

mat2 mm2(float a){float c = cos(a), s = sin(a);return mat2(c,-s,s,c);}
float tri( float x){return abs(fract(x)-.5);}
vec2 tri2(vec2 p){return vec2(tri(p.x+tri(p.y*2.)),tri(p.y+tri(p.x*2.)));}
mat2 m2 = mat2( 0.970,  0.242, -0.242,  0.970 );

//Animated triangle noise, cheap and pretty decent looking.
float triangleNoise(vec2 p)
{
    float z=1.5;
    float z2=1.5;
	float rz = 0.;
    vec2 bp = p;
	for (float i=0.; i<=3.; i++ )
	{
        vec2 dg = tri2(bp*2.)*.8;
        dg *= mm2(time*.003);
        p += dg/z2;

        bp *= 1.6;
        z2 *= .6;
		z *= 1.8;
		p *= 1.2;
        p*= m2;
        
        rz+= (tri(p.x+tri(p.y)))/z;
	}
	return rz;
}

void main()
{
    colorValues[0] = texture(tex0, gl_FragCoord.xy);// * vec4(1.0,0.0,0,1.0);
    colorValues[1] = texture(tex1, gl_FragCoord.xy);// * vec4(0.0,1.0,0,1.0);;
    colorValues[2] = texture(tex2, gl_FragCoord.xy);// * vec4(0.0,0,1,1.0);;
    colorValues[3] = texture(tex3, gl_FragCoord.xy);// * vec4(1.0,1,0,1.0);;
    colorValues[4] = texture(tex4, gl_FragCoord.xy);// * vec4(0.0,1,1,1.0);
    colorValues[5] = texture(tex5, gl_FragCoord.xy);// * vec4(0.0,1,1,1.0);
    colorValues[6] = texture(tex6, gl_FragCoord.xy);// * vec4(0.0,1,1,1.0);
    colorValues[7] = texture(tex7, gl_FragCoord.xy);// * vec4(0.0,1,1,1.0);

    vec4 color = vec4(0, 0, 0, 0);
    for(int i=0;i<nTex-1;i++){
    	color += colorValues[i]*(colorValues[i].a) + colorValues[i+1]*(1-colorValues[i].a);
    	//color = mix(colorValues[i],colorValues[i+1],0.5);
    	//color += (colorValues[i]);
    }
    //color = abs(color);

    float dist = distance(gl_FragCoord.xy,vec2(width*0.5,width*0.5));
    if(dist > width*0.5) color = vec4(0,0,0,0);
    //color.a *= (0.5 + 0.5*triangleNoise(gl_FragCoord.xy/500));

    outputColor = color;
}