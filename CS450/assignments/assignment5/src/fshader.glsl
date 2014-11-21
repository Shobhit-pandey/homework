#version 150

in vec4 vColor;
in vec3 pos;
in vec3 normal;
in vec3 light;

uniform int Swap;

// Define Illumination Constants
vec3 light_ambient  = vec3(0.2);
vec3 light_diffuse  = vec3(1.0);
vec3 light_specular = vec3(1.0);

vec3 material_ambient  = vec3(1.0, 0.0, 1.0);
vec3 material_diffuse  = vec3(1.0, 0.8, 0.0);
vec3 material_specular = vec3(1.0, 0.8, 0.0);

out vec4 color;

void main()
{
    vec3 L = normalize(light - pos);
    vec3 N = normalize(normal);
    vec3 R = reflect(-L, N);
    vec3 V = normalize(-pos);

    //vec3 normal;
    //vec3 light;
    //vec3 viewer;
    //vec3 reflection;

    // Transform vertex normal into eye coordinates
    //   To correctly transform normals use first formation
    // This should just be view, that way when the model rotates, the
    // light does not.
    // vec3 N = (normalize (transpose (inverse (ModelView))*vNormal).xyz);

    vec3 ambient = light_ambient * material_ambient;
    vec3 diffuse = max(dot(L, N), 0.0) * light_diffuse * material_diffuse;
    vec3 specular = pow(max(dot(R, V), 0.0), 100.0) * light_specular * material_specular;

    if (Swap == 0) {
        vec3 light_color = ambient + diffuse + specular;
        color = vec4(light_color, 1.0);
    } else {
        color = vColor;
    }
}
