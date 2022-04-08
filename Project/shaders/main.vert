#version 460 core

// In
in layout(location = 0) vec3 in_position;
in layout(location = 1) vec3 in_normal;
in layout(location = 2) vec3 in_tangent;
in layout(location = 3) vec3 in_bitangent;
in layout(location = 4) vec2 in_texture_coordinates;

uniform layout(location = 1) mat4 M; // Model Matrix
uniform layout(location = 2) mat4 V; // View Matrix
uniform layout(location = 3) mat4 P; // Projection Matrix
uniform layout(location = 4) mat3 N; // Normal Matrix



// Out
out layout(location = 1) vec3 out_fragment_position;
out layout(location = 2) vec3 out_normal;
out layout(location = 3) vec2 out_texture_coordinates;
out layout(location = 4) mat3 TBN; // Tangent Bitangent Normal Matrix



void main()
{
    gl_Position = P * V * M * vec4(in_position, 1);

    // Pass values to Fragment Shader
    out_fragment_position   = vec3(M * vec4(in_position, 1));
    out_normal              = normalize(N * in_normal);
    out_texture_coordinates = in_texture_coordinates;

    // Construct TBN matrix
    vec3 tangent   = normalize(N * in_tangent);
    vec3 bitangent = normalize(N * in_bitangent);
    vec3 normal    = normalize(N * in_normal);
    TBN            = mat3(tangent, bitangent, normal);
}