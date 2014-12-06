#version 150

in vec4 vPosition;

uniform mat4 Transform;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
    gl_Position = Projection * View * Transform * vPosition;
}
