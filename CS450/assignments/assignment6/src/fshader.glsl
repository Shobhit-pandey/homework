#version 150

in vec3 pos;
in vec3 normal;
in vec3 light;
in vec4 fColor;

in vec4 shadowVec;

// Define Illumination Constants
vec3 light_ambient  = vec3(0.2);
vec3 light_diffuse  = vec3(1.0);
vec3 light_specular = vec3(1.0);

vec3 material_ambient  = vec3(1.0, 0.0, 1.0);
vec3 material_diffuse  = vec3(1.0, 0.8, 0.0);
vec3 material_specular = vec3(1.0, 0.8, 0.0);

uniform sampler2DShadow shadowMap;

out vec4 color;

void main()
{
    vec3 L = normalize(light - pos); // vector from point to light
    vec3 N = normalize(normal); // who can know for sure if these are normal?
    vec3 R = reflect(-L, N); // Relfect around normal. (Positive would highlight shadows)
    vec3 V = normalize(-pos); // vector towards the viewer

    vec3 ambient = light_ambient * material_ambient;
    vec3 diffuse = max(dot(L, N), 0.0) * light_diffuse * material_diffuse;
    vec3 specular = pow(max(dot(R, V), 0.0), 100.0) * light_specular * material_specular;

    // Look up shadow depth, and compare to current depth
    float visibility = 1.0;
    if( texture(shadowMap, shadowVec.xy).z < shadowVec.z) {
        visibility = 0.5;
    }
    //float visibility = texture(shadowMap, shadowVec.xyz);
    //float visibility = 0.5;

    vec3 light_color = ambient + (visibility * diffuse) + (visibility * specular);
    //vec3 light_color = ambient +  diffuse + specular;
    color = vec4(light_color, 1.0);
}
