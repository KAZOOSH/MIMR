#version 150


uniform sampler2D tex;
in vec2 texCoordVarying;

out vec4 outputColor;

void main (void) {
    
    outputColor = texture(tex, texCoordVarying);
    
}