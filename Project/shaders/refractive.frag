#version 460 core

// In
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

// Uniforms
uniform layout(location = 10) vec3 camera_position;
uniform layout(location = 11) bool has_textures;

// Textures
uniform layout(binding = 0) samplerCube skybox;
uniform layout(binding = 2) sampler2D normal_map;

// Out
out vec4 fragment_color;



// Fresnel is the effect of you see through water if you look directly
// down, but you see reflection of the sun when looking towards the horizon.
// (it just weighs the contribution of refraction and reflection based on the normal)
vec3 fresnel(vec3 N)
{
    vec3 I = normalize(in_fragment_position - camera_position);

    float index_of_refraction = 1.0 / 1.53;

    // Calculate Refraction
    //
    //     I  N
    //      \ |
    //       \|
    //    ---------- surface
    //          \      
    //           \ Refract

    vec3 Refract    = refract(I, N, index_of_refraction);
    float r         = texture(skybox, Refract + 0.0015).r;
    float g         = texture(skybox, Refract).g;
    float b         = texture(skybox, Refract - 0.0015).b;
    vec3 refraction = vec3(r, g, b);



    // Calculate Reflection
    //
    //      I  N  Reflect
    //       \ | /
    //        \|/
    //    ------------ surface

    vec3 Reflect    = reflect(I, N);
    vec3 reflection = texture(skybox, Reflect).rgb;

    // Calculate Fresnel
    //
    //     I  N  Reflect
    //      \ | /
    //       \|/
    //    ---------- surface
    //          \      
    //           \ Refract


    float ratio = pow(dot(-I, N), 0.25);
    return mix(reflection, refraction, ratio);
}



void main()
{
    vec3 normal = normalize(in_normal);

    if (has_textures)
    {
        normal = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
    }

    fragment_color = vec4(fresnel(normal), 1);
}