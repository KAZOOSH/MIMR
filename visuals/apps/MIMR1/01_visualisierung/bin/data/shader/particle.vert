#version 150

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 texCoordVarying;

void main() {
	texCoordVarying = vec2(texcoord.x, texcoord.y);

    gl_Position = modelViewProjectionMatrix * position;


	//float size    = gl_Normal.x;
    gl_PointSize  = 10;
  //  gl_FrontColor = gl_Color;

}