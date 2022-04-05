#version 430 core

// Definitions
#define num_lights 4
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
in layout(location = 0) vec3 in_normal;
in layout(location = 1) vec2 in_texture_coordinates;
in layout(location = 2) vec3 in_fragment_position;

uniform vec3 camera_position;
uniform Light lights[num_lights];
uniform Ball ball;

// Out
out vec4 color;



/**
Find the amount of shadow (from the ball) that covers this fragment from a specific light.
return 1.0 if there is 100% shadow, 0 if there is no shadow
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
float ambient_strength  = 0.4; // How bright are areas where there is no light
float specular_strength = 0.7; // Shininess Factor

float attenuation_a = 0.100;
float attenuation_b = 0.010;
float attenuation_c = 0.001;

vec3 phong_lighting(vec3 surface_normal)
{
    vec3 ambient;  // Lighting for areas where there is no light
    vec3 diffuse;  // Lighting for entire surface based on if it is facing the light source
    vec3 specular; // Highlist that reflect the light back into the camera

    // Add each lights contribution for this fragment
    for (int i = 0; i < num_lights; i++)
    {
        Light light = lights[i];

        // Ambient
        ambient += light.color * (ambient_strength / num_lights); // Ambient color as combination of all light colors in the scene

        float shadow_factor = 1 - shadow_from(light);

        // Light Attenuation (factor to darken light the further away from the lightsource you are)
        float distance_to_light = distance(light.position, in_fragment_position);
        float attenuation       = 1 / (attenuation_a + attenuation_b * distance_to_light + attenuation_c * pow(distance_to_light, 2));

        // Diffuse
        vec3 light_direction = normalize(light.position - in_fragment_position); // Vector from fragment to light
        diffuse += light.color * attenuation * shadow_factor * max(dot(surface_normal, light_direction), 0);

        // Specular
        vec3 reflection_direction = reflect(-light_direction, surface_normal);         //
        vec3 camera_direction     = normalize(camera_position - in_fragment_position); // Vector from fragment to camera
        specular += light.color * attenuation * shadow_factor * specular_strength * pow(max(dot(camera_direction, reflection_direction), 0.0), 32);
    }

    // Merge it all into the total poung lighting
    return ambient + diffuse + specular;
}



void main()
{
    vec3 normal        = normalize(in_normal);
    vec3 surface_color = vec3(1);

    vec3 phong_lighting = phong_lighting(normal);
    float dithering     = dither(in_texture_coordinates);

    vec3 fragment_color = phong_lighting * surface_color + dithering;

    color = vec4(fragment_color, 1);
}