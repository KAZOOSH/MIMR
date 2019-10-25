#version 150

uniform sampler2D tex;
uniform int colorSet;
in  float		rotAngle;

in vec4 colorVarying;

out vec4 fragColor;

void main (void) {
	//fragColor = texture(tex, gl_PointCoord) * colorVarying;
	//fragColor = vec4(1,0,0,1);

	float angle		= -rotAngle;
	vec2 offset		= vec2(.5, .5);
	vec2 v2 = gl_PointCoord;	
	
	// not sure this is right
	// I just search on how to rotate
	// a texture...
	float cosX, sinX;
	float cosY, sinY;
	sinX = sin(angle);
	cosX = cos(angle);
	sinY = sin(angle);
	cosY = cos(angle);
	
	mat2 rotationMatrix = mat2( cosX, -sinX,
							    sinY, cosX);
	
	vec2 newcoords = ((v2-offset) * (rotationMatrix));
	newcoords += offset;
	
	vec4 color = texture(tex, newcoords) * colorVarying;
	
	fragColor = color;
}
