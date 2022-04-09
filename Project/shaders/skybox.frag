#version 460 core



// From skybox.vert
in layout(location = 1) vec3 in_position;

// Textures
uniform samplerCube skybox;



// Output
out vec4 fragment_color;



void main()
{
    fragment_color = texture(skybox, in_position);
}