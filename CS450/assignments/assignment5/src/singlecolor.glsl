#version 150

in vec3 pos;
in vec3 normal;
in vec3 light;
in vec4 fColor;

out vec4 color;

void main()
{
    color = fColor;
}
