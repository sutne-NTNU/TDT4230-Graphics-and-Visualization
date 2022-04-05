#version 430 core

#define GEOMETRY_2D 0
#define GEOMETRY_3D 1
#define GEOMETRY_TEXTURED 2

// In
in layout(location = 0) vec3 in_position;
in layout(location = 1) vec3 in_normal;
in layout(location = 2) vec3 in_tangent;
in layout(location = 3) vec3 in_bitangent;
in layout(location = 4) vec2 in_texture_coordinates;

uniform layout(location = 0) int geometry_type; // 2D/3D/NormalMapped object
uniform layout(location = 1) mat4 M;            // Model Transformation Matrix
uniform layout(location = 2) mat4 MVP;          // ModelViewProjection Transformation Matrix
uniform layout(location = 3) mat3 N;            // Normal (Transformation) Matrix
uniform layout(location = 4) mat4 O;            // Orthogonal Matrix for 2D objects

// Out
out layout(location = 0) flat int out_geometry_mode;
out layout(location = 1) vec3 out_fragment_position;
out layout(location = 2) vec3 out_normal;
out layout(location = 3) vec2 out_texture_coordinates;
out layout(location = 4) mat3 TBN; // Tangent Bitangent Normal Matrix



void main()
{
    if (geometry_type == GEOMETRY_2D)
    {
        gl_Position = O * M * vec4(in_position, 1);
    }
    else
    {
        gl_Position = MVP * vec4(in_position, 1);
    }

    // Pass values to Fragment Shader
    out_geometry_mode       = geometry_type;
    out_fragment_position   = vec3(M * vec4(in_position, 1));
    out_normal              = normalize(N * in_normal);
    TBN                     = mat3(normalize(in_tangent), normalize(in_bitangent), normalize(in_normal));
    out_texture_coordinates = in_texture_coordinates;
}
