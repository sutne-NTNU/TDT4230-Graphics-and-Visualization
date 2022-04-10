#version 460 core



// From Vertex Shader
in layout(location = 1) vec3 in_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

// Uniforms
uniform layout(location = 10) bool has_textures;
uniform layout(location = 11) vec3 camera_position;

// Textures
uniform layout(binding = 0) samplerCube skybox;
uniform layout(binding = 2) sampler2D normal_map;

// Values
float refraction_index  = 1.53; // Glass
float dispersion_factor = 0.00175;



// Output
out vec4 fragment_color;



// Fresnel is a combination of refraction and reflection based
// on the type of material, the way a window is transparent when looking
// directly at it, but from an angle it works as a mirror.
//
// I also include chromatic aberration, which is that different wavelengths
// of light have a different refraction index, hence why you get rainbow light
// when it hits glass just right.
vec3 fresnel(vec3 N)
{
    vec3 I = normalize(in_position - camera_position);

    float index_of_refraction = 1.0 / refraction_index;

    // Calculate Reflection
    //
    //      I  N  Reflect
    //       \ | /
    //        \|/
    //    ------------ surface

    vec3 Reflect    = reflect(I, N);
    vec3 reflection = texture(skybox, Reflect).rgb;

    // Calculate Refraction
    //
    //     I  N
    //      \ |
    //       \|
    //    ---------- surface
    //          \      
    //           \ Refract

    vec3 Refract    = refract(I, N, index_of_refraction);
    float r         = texture(skybox, Refract + dispersion_factor).r;
    float g         = texture(skybox, Refract).g;
    float b         = texture(skybox, Refract - dispersion_factor).b;
    vec3 refraction = vec3(r, g, b);

    // Calculate Fresnel
    //
    //     I  N  Reflect * (ratio)
    //      \ | /
    //       \|/
    //    ---------- surface
    //          \      
    //           \ 
    //           Refract * (1 - ratio)

    float ratio = pow(dot(-I, N), index_of_refraction);
    return mix(reflection, refraction, ratio);
}



void main()
{
    vec3 normal = normalize(in_normal);

    if (has_textures)
    {
        // normal = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
    }

    fragment_color = vec4(fresnel(normal), 1.0);
}