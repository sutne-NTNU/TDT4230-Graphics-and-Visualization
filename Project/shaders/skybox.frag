#version 460 core

// In
in layout(location = 1) vec3 in_fragment_position;

// Textures
uniform layout(binding = 0) samplerCube skybox;



// Out
out vec4 fragment_color;



void main()
{
    fragment_color = texture(skybox, in_fragment_position);
}