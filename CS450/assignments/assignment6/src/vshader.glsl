#version 150

in  vec4 vColor;
in  vec4 vPosition;
in  vec4 vNormal;

uniform mat4 Transform;
uniform mat4 View;
uniform mat4 Projection;

vec4 vLight = vec4(1.5, 1.5, 2.0, 1.0);

out vec3 pos;
out vec3 normal;
out vec3 light;
out vec4 fColor;

void main()
{
    // Transform position, normals, and light into eye coordinates
    pos    = vec3(View * vPosition);
    normal = vec3(transpose(inverse(View * Transform))*vNormal );
    light  = vec3(View * vLight);

    gl_Position = Projection * View * Transform * vPosition;
    fColor = vColor;
}
