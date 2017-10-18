#version 150

#define M_PI 3.1415926535897932384626433832795

// this is how we receive the texture
uniform sampler2DRect tex0;

uniform float angle;

int spriteMode = 1;


in vec2 texCoordVarying;

out vec4 outputColor;
 

 int lengthSprite = 1;


void main()
{
	vec4 color = vec4(0,0,0,0);



   /* vec2 dParticle = vec2(300000,300000);
    for(int x=0; x< lengthSprite; x++){
        for(int y=0; y< lengthSprite; y++){
            if (texture(tex0, texCoordVarying + vec2(x,y)).r > 0.0){
                if(x < dParticle.x) dParticle.x = x;
                if(y < dParticle.y) dParticle.y = y;
                color.a = texture(tex0, texCoordVarying + vec2(x,y)).r;
            }
        }
    }

    float d = 1- min(dParticle.x,dParticle.y)/lengthSprite;*/

    

    color.r = 1.0;
    color.g = 1.0;
    color.b = 1.0;
    color.a = texture(tex0, texCoordVarying).a;

    outputColor = color;
}

