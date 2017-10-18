#version 150

uniform mat4 orientationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform float spriteSize;

in vec4  position;
in vec4  color;
in vec3  normal;

out vec4 colorVarying;
out float rotAngle;


void main() {
	vec4 eyeCoord = modelViewMatrix * position;
	gl_Position = projectionMatrix * eyeCoord; // to be accurate this should premultiply by orientationMatrix
	
	float dist = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float att	 = 600.0 / dist;
	
	gl_PointSize = normal.x;// * att;
	colorVarying = color;
	rotAngle = normal.y;
}
/*
#version 150

uniform mat4 orientationMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in float		pointSize;
in float		angle;

in vec4  position;

out float rotAngle;

void main() {
	
	rotAngle = angle;

	vec4 eyeCoord  = modelViewMatrix * position;
	gl_Position    = modelViewProjectionMatrix * position;
	float dist     = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float att	   = 600.0 / dist;
	gl_PointSize   = pointSize * att;	
	
	
}*/





