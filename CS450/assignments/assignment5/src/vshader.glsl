#version 150

in  vec4 vPosition;
in  vec4 vNormal;
in  vec4 vColor;
out vec4 color;

uniform mat4 Transform;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform int Swap;

void main()
{
    // Define Illumination Constants
    vec3 light_ambient  = vec3(0.2);
    vec3 light_diffuse  = vec3(1.0);
    vec3 light_specular = vec3(1.0);

    vec3 material_ambient  = vec3(1.0, 0.0, 1.0);
    vec3 material_diffuse  = vec3(1.0, 0.8, 0.0);
    vec3 material_specular = vec3(1.0, 0.8, 0.0);

    vec4 LightPosition = vec4(1.5, 1.5, 2.0, 1.0);

    vec3 AmbientProduct  = light_ambient  * material_ambient;
    vec3 DiffuseProduct  = light_diffuse  * material_diffuse;
    vec3 SpecularProduct = light_specular * material_specular;

    //vec3 normal;
    //vec3 light;
    //vec3 viewer;
    //vec3 reflection;

    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;

    vec3 L = normalize( (ModelView * LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );  //halfway vector

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView*vNormal ).xyz;

    //To correctly transform normals
    // vec3      N = (normalize (transpose (inverse (ModelView))*vNormal).xyz

    float dr = max( dot(L, N), 0.0 );
    vec3  diffuse = dr * DiffuseProduct;

    // Power constant represents 'shininess'
    float sr = pow( max(dot(N, H), 0.0), 100.0 );
    vec3 specular = sr * SpecularProduct;

    if( dot(L, N) < 0.0 ) {
        specular = vec3(0.0, 0.0, 0.0);
    }

    gl_Position = Projection * ModelView * Transform * vPosition;

    if (Swap == 0) {
        vec3 light_color = AmbientProduct + diffuse + specular;
        color = vec4(light_color, 1.0);
    } else {
        color = vColor;
    }
}
