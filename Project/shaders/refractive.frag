#version 460 core

// In
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

// Uniforms
uniform layout(location = 10) bool has_textures;
uniform layout(location = 11) vec3 camera_position;
uniform layout(location = 14) bool back_side_pass; // Create normal texture map of backside of objects

// Textures
uniform layout(binding = 0) samplerCube skybox;
uniform layout(binding = 2) sampler2D normal_map;
uniform layout(binding = 4) sampler2D back_side_normal_map;

// Out
out vec4 fragment_color;


// Values
float refraction_index  = 1.53;
float dispersion_factor = 0.0015;


// Fresnel is the effect of you see through water if you look directly
// down, but you see reflection of the sun when looking towards the horizon.
// (it just weighs the contribution of refraction and reflection based on the normal)
vec3 fresnel(vec3 N)
{
    vec3 I = normalize(in_fragment_position - camera_position);

    float index_of_refraction = 1.0 / refraction_index;

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



// Refraction for when light "leaves" the object, aka from glass to air.
vec3 get_back_refraction(vec3 N)
{
    vec3 I                    = normalize(in_fragment_position - camera_position);
    float index_of_refraction = refraction_index / 1.0;

    vec3 Refract = refract(I, N, index_of_refraction);
    float r      = texture(skybox, Refract + dispersion_factor).r;
    float g      = texture(skybox, Refract).g;
    float b      = texture(skybox, Refract - dispersion_factor).b;
    return vec3(r, g, b);
}



void main()
{
    vec3 normal = normalize(in_normal);

    if (has_textures)
    {
        // normal = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
    }


    if (back_side_pass)
    {
        fragment_color = vec4(-normal, 1.0);
    }
    else
    {
        vec2 uv              = vec2(gl_FragCoord.x / 1920, gl_FragCoord.y / 1080);
        vec3 back_normal     = normalize(texture(back_side_normal_map, uv).xyz * 2.0 - 1.0);
        vec3 back_refraction = get_back_refraction(back_normal);

        vec3 front_fresnel = fresnel(normal);
        fragment_color     = vec4(back_normal, 1);
        // fragment_color     = vec4(mix(front_fresnel, back_refraction, 0.5), 1);
    }
}