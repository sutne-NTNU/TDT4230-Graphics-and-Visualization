#version 460 core

// From Vertex Shader
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

// Uniforms
uniform layout(location = 10) bool has_textures;
uniform layout(location = 11) vec3 camera_position;
uniform layout(location = 12) vec3 sunlight_color;
uniform layout(location = 13) vec3 sunlight_direction;

// Textures
uniform layout(binding = 1) sampler2D diffuse_map;
uniform layout(binding = 2) sampler2D normal_map;
uniform layout(binding = 3) sampler2D roughness_map;



// Output
out vec4 fragment_color;



// Global Lighting from the sun based on phong lighting
vec3 sunlight(vec3 N, float rgh)
{
    float shininess_factor    = 5 / pow(rgh, 2);
    vec3 camera_direction     = normalize(camera_position - in_fragment_position);
    vec3 reflection_direction = reflect(sunlight_direction, N);

    vec3 ambient  = sunlight_color * 0.2;
    vec3 diffuse  = sunlight_color * max(dot(N, -sunlight_direction), 0);
    vec3 specular = sunlight_color * pow(max(dot(camera_direction, reflection_direction), 0.0), shininess_factor);
    return ambient + diffuse + specular;
}



void main()
{
    vec3 normal     = normalize(in_normal);
    vec3 color      = vec3(1, 0.11, 0.85);
    float roughness = 0.4;

    // if (has_textures)
    // {
    //     normal    = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
    //     color     = texture(diffuse_map, in_texture_coordinates).rgb;
    //     roughness = texture(roughness_map, in_texture_coordinates).x;
    // }

    fragment_color = vec4(color * sunlight(normal, roughness), 1);
}
