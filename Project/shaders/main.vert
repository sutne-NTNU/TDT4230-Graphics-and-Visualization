#version 460 core

// Different object types
#define SKYBOX 0
#define GEOMETRY_SHAPE 1
#define GEOMETRY_TEXTURED 2

// Different render passes
#define REFRACTION 0 // Only render normals of backside of opaque surfaces
#define REFLECTION 1 // Render scene to what will become a cubemap
#define RENDER 2     // Render entire scene



// In
in layout(location = 0) vec3 in_position;
in layout(location = 1) vec3 in_normal;
in layout(location = 2) vec3 in_tangent;
in layout(location = 3) vec3 in_bitangent;
in layout(location = 4) vec2 in_texture_coordinates;

uniform layout(location = 1) mat4 M;   // Model Transformation Matrix
uniform layout(location = 2) mat4 MVP; // ModelViewProjection Transformation Matrix
uniform layout(location = 3) mat3 N;   // Normal (Transformation) Matrix

uniform layout(location = 10) int TYPE;
uniform layout(location = 11) int PASS;



// Out
out layout(location = 1) vec3 out_fragment_position;
out layout(location = 2) vec3 out_normal;
out layout(location = 3) vec2 out_texture_coordinates;
out layout(location = 4) mat3 TBN; // Tangent Bitangent Normal Matrix
out layout(location = 10) flat int out_TYPE;
out layout(location = 11) flat int out_PASS;



void main()
{
    if (TYPE == SKYBOX)
    {
        gl_Position = MVP * vec4(in_position, 1);

        out_fragment_position = in_position;
    }



    if (TYPE == GEOMETRY_SHAPE || TYPE == GEOMETRY_TEXTURED)
    {
        gl_Position = MVP * vec4(in_position, 1);

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

    out_TYPE = TYPE;
    out_PASS = PASS;
}