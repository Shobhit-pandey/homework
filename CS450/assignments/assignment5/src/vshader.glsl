#version 150

in  vec4 vColor;
in  vec4 vPosition;
in  vec4 vNormal;

uniform mat4 Transform;
uniform mat4 ModelView;
uniform mat4 Projection;

vec4 vLight = vec4(1.5, 1.5, 2.0, 1.0);

out vec3 pos;
out vec3 normal;
out vec3 light;
out vec4 fColor;

void main()
{
    // Transform position, normals, and light into eye coordinates
    pos    = vec3(ModelView * vPosition);
    normal = vec3(transpose(inverse(ModelView))*vNormal );
    light  = vec3(ModelView * vLight);

    gl_Position = Projection * ModelView * Transform * vPosition;
    fColor = vColor;
}
