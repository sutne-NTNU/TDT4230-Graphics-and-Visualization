#version 460 core

// In
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

// uniforms
uniform layout(location = 10) bool has_textures;
uniform layout(location = 11) vec3 camera_position;

// Textures
uniform layout(binding = 0) samplerCube skybox;
uniform layout(binding = 2) sampler2D normal_map;

// Out
out vec4 fragment_color;



// Reflection from environment
vec3 get_reflection(vec3 N)
{
    //     I   N   R
    //      \  |  /
    //       \ | /
    //        \|/
    //    ------------ surface
    vec3 I = normalize(in_fragment_position - camera_position);
    vec3 R = reflect(I, N);
    return texture(skybox, R).rgb;
}



void main()
{
    vec3 normal = normalize(in_normal);

    if (has_textures)
    {
        // normal = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
    }

    fragment_color = vec4(get_reflection(normal), 1);
}
