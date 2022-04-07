#version 460 core

// Different object handling modes
#define SKYBOX 0
#define GEOMETRY_SHAPE 1
#define GEOMETRY_TEXTURED 2

// Different render passes
#define REFRACTION 0 // Only render normals of backside of opaque surfaces
#define REFLECTION 1 // Render scene to what will become a cubemap
#define RENDER 2     // Render entire scene

#define DEBUG_1 10
#define DEBUG_2 11

// Definitions
struct Camera
{
    vec3 position;
};

struct Sun
{
    vec3 color;
    vec3 direction; // From sun
};

struct Appearance
{
    bool use_texture_map;
    vec3 color;
    float opacity;
    float roughness;
    float reflectivity;
    float refraction_index;
};


// In
in layout(location = 1) vec3 in_fragment_position;
in layout(location = 2) vec3 in_normal;
in layout(location = 3) vec2 in_texture_coordinates;
in layout(location = 4) mat3 TBN;

in layout(location = 10) flat int TYPE;
in layout(location = 11) flat int PASS;


uniform Camera camera;
uniform Appearance appearance;
uniform Sun sun;

// Resource textures
uniform layout(binding = 0) samplerCube skybox;
uniform layout(binding = 1) sampler2D texture_map;
uniform layout(binding = 2) sampler2D normal_map;
uniform layout(binding = 3) sampler2D roughness_map;

// Created textures from framebuffers
uniform layout(binding = 10) samplerCube reflection_cubemap;
uniform layout(binding = 11) sampler2D refraction_backface_normal_map;


uniform sampler2D debugTexture;

// Out
out vec4 fragment_color;



// Global Lighting from the sun based on phong lighting
vec3 sunlight(float rgh, vec3 N)
{
    float shininess_factor    = 5 / pow(rgh, 2);
    vec3 camera_direction     = normalize(camera.position - in_fragment_position);
    vec3 reflection_direction = reflect(sun.direction, N);

    vec3 ambient  = sun.color * 0.3; // Brightness where no light hits
    vec3 diffuse  = sun.color * max(dot(N, -sun.direction), 0);
    vec3 specular = sun.color * pow(max(dot(camera_direction, reflection_direction), 0.0), shininess_factor);
    return ambient + diffuse + specular;
}



// Reflection from environment
vec3 get_reflection(vec3 N)
{
    //     I   N   R
    //      \  |  /
    //       \ | /
    //        \|/
    //    ------------ surface
    vec3 I = normalize(in_fragment_position - camera.position);
    vec3 R = reflect(I, N);
    return texture(skybox, R).rgb;
}



// Refraction after passing once through this surface
// To simulate chromatic abberition i just slight adjust
// the refraction ratio for each color channel.
vec3 get_refraction(vec3 N)
{
    //     I  N
    //      \ |
    //       \|
    //    ---------- surface
    //          \      
    //           \ R

    float ratio = 1.0 / appearance.refraction_index;
    vec3 I      = normalize(in_fragment_position - camera.position);
    vec3 R      = refract(I, N, ratio);

    float r = texture(skybox, R + 0.0015).r;
    float g = texture(skybox, R).g;
    float b = texture(skybox, R - 0.0015).b;
    return vec3(r, g, b);
}



// C - Original surface color
// R - Roughness
// N - Normal
vec3 get_surface_color(vec3 N, vec3 C, float R)
{
    return C * sunlight(R, N);
}



// Fresnel is the effect of you see through water if you look directly
// down, but you see reflection of the sun when looking towards the horizon.
// (it just weighs the contribution of refraction and reflection based on the normal)
vec3 apply_fresnel(vec3 reflection, vec3 refraction, vec3 normal)
{
    float Eta   = 1.0 / appearance.refraction_index;
    float F     = ((1.0 - Eta) * (1.0 - Eta)) / ((1.0 + Eta) * (1.0 + Eta));
    vec3 I      = normalize(in_fragment_position - camera.position);
    float ratio = F + (1.0 - F) * pow((1.0 - dot(-I, normal)), 5.0);
    return mix(refraction, reflection, ratio);
}



/////////////////////////////////////////////////////////////////////////////////////////
//                                       MAIN                                          //
/////////////////////////////////////////////////////////////////////////////////////////



void main()
{
    if (PASS == REFRACTION)
    {
        // Everything should be (0,0,0,1) except the backside of the objects that are opaque.
        if (appearance.opacity > 0)
        {
            vec3 normal = normalize(in_normal);
            if (TYPE == GEOMETRY_TEXTURED)
            {
                normal = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
            }
            fragment_color = vec4(normal, 1);
        }
        else
        {
            fragment_color = vec4(0, 0, 0, 1);
        }
        return;
    }



    if (PASS == REFLECTION)
    {
        // Optimzation could be to not render as detailed in the reflection pass
        if (TYPE == SKYBOX)
        {
            fragment_color = texture(skybox, in_fragment_position);
            return;
        }



        // For regular geometry
        vec3 normal     = normalize(in_normal);
        vec3 color      = appearance.color;
        float roughness = appearance.roughness;



        // Textured Geometry
        if (TYPE == GEOMETRY_TEXTURED)
        {
            normal    = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
            roughness = texture(roughness_map, in_texture_coordinates).x;

            if (appearance.use_texture_map)
            {
                color = texture(texture_map, in_texture_coordinates).rgb;
            }
        }



        if (appearance.opacity > 0) // light passes through the object
        {
            vec3 reflection = get_reflection(normal);
            vec3 refraction = get_refraction(normal);
            fragment_color  = vec4(apply_fresnel(reflection, refraction, normal), 1);
        }
        else if (appearance.reflectivity > 0)
        {
            fragment_color = vec4(get_reflection(normal), 1);
        }
        else
        {
            fragment_color = vec4(get_surface_color(normal, color, roughness), 1);
        }
    }



    if (PASS == RENDER)
    {
        // Optimzation could be to not render as detailed in the reflection pass
        if (TYPE == SKYBOX)
        {
            fragment_color = texture(skybox, in_fragment_position);
            return;
        }



        // For regular geometry
        vec3 normal     = normalize(in_normal);
        vec3 color      = appearance.color;
        float roughness = appearance.roughness;



        // Textured Geometry
        if (TYPE == GEOMETRY_TEXTURED)
        {
            normal    = normalize(TBN * (texture(normal_map, in_texture_coordinates).xyz * 2 - 1));
            roughness = texture(roughness_map, in_texture_coordinates).x;

            if (appearance.use_texture_map)
            {
                color = texture(texture_map, in_texture_coordinates).rgb;
            }
        }



        if (appearance.opacity > 0) // light passes through the object
        {
            vec3 reflection = get_reflection(normal);
            vec3 refraction = get_refraction(normal);
            fragment_color  = vec4(apply_fresnel(reflection, refraction, normal), 1);
        }
        else if (appearance.reflectivity > 0)
        {
            vec3 I         = normalize(in_fragment_position - camera.position);
            vec3 R         = reflect(I, normal);
            fragment_color = vec4(texture(reflection_cubemap, R).rgb, 1);
        }
        else
        {
            fragment_color = vec4(get_surface_color(normal, color, roughness), 1);
        }
    }
}
