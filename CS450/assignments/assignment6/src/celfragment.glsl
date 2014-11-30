#version 150

in vec3 pos;
in vec3 normal;
in vec3 light;
in vec4 fColor;

uniform int disks;

// Define Illumination Constants
vec3 light_ambient  = vec3(0.2);
vec3 light_diffuse  = vec3(1.0);
vec3 light_specular = vec3(1.0);

vec3 material_ambient  = vec3(1.0, 0.0, 1.0);
vec3 material_diffuse  = vec3(1.0, 0.8, 0.0);
vec3 material_specular = vec3(1.0, 1.0, 1.0);

out vec4 color;

float discratize(float normal_diffuse) {
    if (disks == 2) {
        if (normal_diffuse <= 0.4) {
            return 0.25;
        }
    } else if (disks == 3) {
        if (normal_diffuse <= 0.4) {
            return 0.25;
        } else if (normal_diffuse <= 0.6) {
            return 0.5;
        }
    } else if (disks == 4) {
        if (normal_diffuse <= 0.4) {
            return 0.25;
        } else if (normal_diffuse <= 0.6) {
            return 0.5;
        } else if (normal_diffuse <= 0.8) {
            return 0.75;
        }
    }
    return 0.8;
}

void main()
{
    vec3 L = normalize(light - pos); // vector from point to light
    vec3 N = normalize(normal); // who can know for sure if these are normal?
    vec3 R = reflect(-L, N); // Relfect around normal. (Positive would highlight shadows)
    vec3 V = normalize(-pos); // vector towards the viewer

    float normal_diffuse = max(dot(L,N), 0.0);

    vec3 ambient = light_ambient * material_ambient;
    vec3 diffuse = discratize(normal_diffuse) * light_diffuse * material_diffuse;
    vec3 specular = pow(max(dot(L, R), 0.0), 1024.0) * light_specular * material_specular;

    float sill = max(dot(V, N), 0.0);
    if ( sill <= 0.2f ) {
        color = vec4(0.1, 0.1, 0.1, 1.0);
    } else {
        color = vec4(ambient + diffuse + specular, 1.0);
    }
}
