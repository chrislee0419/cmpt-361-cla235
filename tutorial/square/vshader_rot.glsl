#version 130

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform float theta; // Rotation around z axis (in degree)
uniform vec2  scale; // Scales along x and y axes.

void main() 
{
    // Rotation around z axis
    float angles = radians( theta );
    float c = cos( angles );
    float s = sin( angles );
    mat4 rz = mat4(   c,  -s, 0.0, 0.0,
					  s,   c, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0 );
	
	// Scale along x and y axis
	float sx = scale[0];
	float sy = scale[1];
	mat4 sxy = mat4( sx, 0.0, 0.0, 0.0,
					0.0,  sy, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0 );
   
	// Output vertex position
    gl_Position = sxy * rz * vPosition;
	
	// Output vertex color
	color = vColor;
} 
