#version 460 core



// Attributes
in layout(location = 0) vec3 in_position;

// Uniforms
uniform layout(location = 2) mat4 V; // View Matrix
uniform layout(location = 3) mat4 P; // Projection Matrix



// Output
out layout(location = 1) vec3 out_fragment_position;



void main()
{
    gl_Position           = P * V * vec4(in_position, 1);
    out_fragment_position = in_position;
}