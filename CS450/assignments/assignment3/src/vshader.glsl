#version 150

in  vec4 vPosition;
in  vec4 vNormal;
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;

void main()
{
    // Transform vertex  position into eye coordinates
    vec3 pos = (ModelView * vPosition).xyz;

    vec3 L = normalize( (ModelView * LightPosition).xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );  //halfway vector

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( ModelView*vNormal ).xyz;

    //To correctly transform normals
    // vec3      N = (normalize (transpose (inverse (ModelView))*vNormal).xyz

    // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float dr = max( dot(L, N), 0.0 );
    vec4  diffuse = dr *DiffuseProduct;

    float sr = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = sr * SpecularProduct;

    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    }

    gl_Position = Projection * ModelView * vPosition;

    color = ambient + diffuse + specular;
    color.a = 1.0;
}
