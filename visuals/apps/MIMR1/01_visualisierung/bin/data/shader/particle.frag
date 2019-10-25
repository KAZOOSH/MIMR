#version 150


uniform sampler2D tex;
in vec2 texCoordVarying;

uniform int colorSet;

out vec4 outputColor;

void main (void) {
    
    vec4 color = texture(tex, texCoordVarying);

    if (colorSet == 0)
    {
    	outputColor.r *= 0.04;
    	outputColor.g *= 0.2;
    	outputColor.b *= 1.0;
    }

    outputColor = color;
    
}