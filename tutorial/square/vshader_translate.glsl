#version 130

in vec4 vPosition;
in vec4 vColor;
out vec4 color;

uniform vec4 translation;

void main() 
{
    color = vColor;
    gl_Position = vPosition + translation;
} 
