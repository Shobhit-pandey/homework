#version 150

in vec2 vPosition;
in vec4 vColor;
out vec4 color;

void
main()
{
    gl_Position = vec4(vPosition,0.0,1.0);
    color = vColor;
}
