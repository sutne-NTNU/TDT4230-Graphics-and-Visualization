#version 430 core

#define GEOMETRY_2D 0
#define GEOMETRY_3D 1
#define GEOMETRY_TEXTURED 2

// Definitions
#define num_lights 1
struct Light
{
    vec3 position;
    vec3 color;
};
struct Ball
{
    vec3 position;
    float radius;
};

float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}
float dither(vec2 uv)
{
    return (rand(uv) * 2.0 - 1.0) / 256.0;
}
vec3 reject(vec3 from, vec3 onto)
{
    return from - onto * dot(from, onto) / dot(onto, onto);
}


// In
in layout(location = 0) flat int geometry_type;
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

uniform layout(binding = 0) sampler2D texture_map;
uniform layout(binding = 1) sampler2D normal_map;
uniform layout(binding = 2) sampler2D roughness_map;

uniform vec3 camera_position;
uniform Light lights[num_lights];
uniform Ball ball;

// Out
out vec4 color;



/**
Find the amount of shadow (from the ball) that covers this fragment from a specific light.
return:
    1 if there is 100% shadow
    0 if there is no shadow
*/
float soft_area   = 1;
float soft_radius = ball.radius + soft_area;

float shadow_from(Light light)
{
    vec3 ball_vector  = ball.position - in_fragment_position;  // Vector from fragment to center of the ball
    vec3 light_vector = light.position - in_fragment_position; // Vector from fragment to the light

    if (length(light_vector) < length(ball_vector)) return 0; // Light is in front of the ball
    if (dot(ball_vector, light_vector) < 0) return 0;         // Light and ball are in oppsite directions

    float rejection_length = length(reject(ball_vector, light_vector));

    if (rejection_length < ball.radius) return 1;        // 100% shadow
    if (rejection_length > soft_radius) return 0;        // 0% shadow (outside soft area)
    return (soft_radius - rejection_length) / soft_area; // Partial/Soft Shadow
}



/** Phong Lighting Explained: https://learnopengl.com/Lighting/Basic-Lighting */
// Constants
float ambient_strength = 0.1; // How bright are areas where there is no light

float attenuation_a = 0.400;
float attenuation_b = 0.010;
float attenuation_c = 0.001;

vec4 phong_lighting(vec3 surface_normal, float roughness)
{
    vec3 ambient  = vec3(0); // Lighting for areas where there is no light
    vec3 diffuse  = vec3(0); // Lighting for entire surface based on if it is facing the light source
    vec3 specular = vec3(0); // Highlist that reflect the light back into the camera

    float shininess_factor = 5 / pow(roughness, 2); // Shininess Factor

    // Vector from fragment to camera
    vec3 camera_direction = normalize(camera_position - in_fragment_position);
    // Add each lights contribution for this fragment
    for (int i = 0; i < num_lights; i++)
    {
        Light light = lights[i];

        vec3 light_direction      = normalize(light.position - in_fragment_position);
        vec3 reflection_direction = reflect(-light_direction, surface_normal);
        float distance_to_light   = distance(light.position, in_fragment_position);

        float brightness  = 1 - shadow_from(light);
        float attenuation = 1 / (attenuation_a + attenuation_b * distance_to_light + attenuation_c * pow(distance_to_light, 2));

        ambient += light.color * (ambient_strength / num_lights); // Ambient color as combination of all light colors in the scene
        diffuse += light.color * attenuation * brightness * max(dot(surface_normal, light_direction), 0);
        specular += light.color * attenuation * brightness * pow(max(dot(camera_direction, reflection_direction), 0.0), shininess_factor);
    }
    // Merge it all into the total poung lighting
    return vec4(ambient + diffuse + specular, 1);
}



void main()
{
    vec3 normal        = normalize(in_normal);   // Normal of Entire Surface (Triangle)
    vec4 surface_color = vec4(0.5, 0.5, 0.5, 1); // Default color of the fragment
    float roughness    = 0.4;                    // Gives Shininess Factor = approx 32 in phong

    color = vec4(0); // The resulting color of the fragment after texturing/lighting
    switch (geometry_type)
    {
        case GEOMETRY_2D: // Text
            color = texture(texture_map, in_texture_coordinates);
            break;


        case GEOMETRY_3D: // Ball and Pad

            color = phong_lighting(normal, roughness) * surface_color + dither(in_texture_coordinates);
            break;


        case GEOMETRY_TEXTURED: // Box

            surface_color = texture(texture_map, in_texture_coordinates);
            normal        = TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1);
            roughness     = texture(roughness_map, in_texture_coordinates).x;

            color = phong_lighting(normal, roughness) * surface_color + dither(in_texture_coordinates);
            break;
    }
}