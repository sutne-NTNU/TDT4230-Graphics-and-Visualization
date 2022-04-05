#version 430 core

// In
in layout(location = 0) vec3 in_position;
in layout(location = 1) vec3 in_normal;
in layout(location = 2) vec2 in_texture_coordinates;
uniform layout(location = 3) mat4 M;   // Model Transformation Matrix
uniform layout(location = 4) mat4 MVP; // ModelViewProjection Transformation Matrix
uniform layout(location = 5) mat3 N;   // Normal (Transformation) Matrix

// Out
out layout(location = 0) vec3 out_normal;              // Normal Vector of the vertex
out layout(location = 1) vec2 out_texture_coordinates; //
out layout(location = 2) vec3 out_fragment_position;   // To Check position relative to lights



void main()
{
    gl_Position = MVP * vec4(in_position, 1.0f);

    // Pass values to Fragment Shader
    out_normal              = normalize(N * in_normal);
    out_texture_coordinates = in_texture_coordinates;
    out_fragment_position   = vec3(M * vec4(in_position, 1));
}
