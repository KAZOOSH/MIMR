#version 150

uniform sampler2DRect tex;

uniform int width;
uniform int height;

uniform float time;
uniform float intensity;

out vec4 outputColor;


float distance(vec2 v1,vec2 v2){
	return length(v2-v1);
}

float distance(float v1,float v2){
	return abs(v2-v1);
}

float angle(vec2 v1, vec2 v2){
	return 12.0;
}

float cosAngle(vec2 v1, vec2 v2){
	return dot(v1,v2)/(length(v1)*length(v2));
}

float map(float value, float min1,float max1,float min2, float max2,bool clamp){
	if (clamp && value <= min1 ) return min2;
	else if (clamp && value >= max1 ) return max2;
	return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float map(float value, float min1,float max1,float min2, float max2){
	return map(value,  min1, max1, min2, max2,false);
}

float mapN(float value, float inMin, float inMax, float outMin, float outMax, float shaper){
	float pct = map (value, inMin, inMax, 0, 1);

	pct = pow(pct, shaper);
	return map(pct, 0,1, outMin, outMax);
}


vec2 ProjectCoordsSphere(vec2 normCoords)
{
    const float SPHERE_RADIUS_SQ = 1.0;
    //z^2 = R^2 - (x^2 + y^2).
    //float z2 = SPHERE_RADIUS_SQ - dot(normCoords, normCoords);
    float z2 = 1 - distance(vec2(0,0),normCoords);

    //if(z2 <= 0.0)
    //    return normCoords;

    //z2 *= 10*intensity;

    //Project the 3D point(normCoords.x, normCoords.y, sqrt(z2)) onto the screen
    //to emulate the sphere-like refraction.
    //vec2 outProjectedCoords = normCoords / sqrt(z2);

    float mult = mapN(z2,0,1,0,1,1.0 + intensity);
    vec2 outProjectedCoords =  normCoords;

    //Add an antialiasing step to avoid jagged edges on the lens.
    const float AA_EDGE = 0.2;
    /*if(z2 < AA_EDGE)
    {
        //Smooth transition of the squared z from 0 to AA_EDGE.
        //Normalize this transition factor to the [0,1] interval.
        float aaFactor = smoothstep(0.0, 1.0, z2 / AA_EDGE);
        
        //Perform another smooth transition between the projected coordinates and the original ones.
        //When z is very small, the projected coordinates are very big and tend to opint to the same position,
        //thus giving the edge of the lens a jagged appearance.
        outProjectedCoords = mix(
            normCoords, 
            outProjectedCoords,
        	aaFactor);
    }*/
    
    return outProjectedCoords;
}

void main()
{
	vec2 uv = gl_FragCoord.xy/vec2(width,height);
	uv -= vec2(0.5,0.5);
	uv*=2;

	vec2 cellDelta = ProjectCoordsSphere(uv);
	vec2 xy = cellDelta + vec2(0.5,0.5);
	vec4 col = texture(tex, xy*vec2(width,height));
	
    outputColor = col;
}