#ifndef APPEARANCE_HPP
#define APPEARANCE_HPP
#pragma once

#include <vector>

#include <glm/glm.hpp>



struct Texture
{
    unsigned int colorID     = -1;
    unsigned int normalID    = -1;
    unsigned int roughnessID = -1;
};


/**
 * Contains the information needed to render a mesh.
 */
struct Appearance
{
    glm::vec3 color;       // Base colour of the material
    float opacity;         // How much light passes thorugh
    float roughness;       // Affects specular highlights
    float refractionIndex; // for light passing through this object
    float reflectivity;    // How reflective is this surface

    bool useTextureMap = false; // Should the diffuse texture be used?
    bool hasTexture    = false; // Does this object have any loaded textures?
    Texture texture;            // Contains Id of the given textures, if they dont exist they are -1

    void setTo(Appearance newAppearance)
    {
        color           = newAppearance.color;
        opacity         = newAppearance.opacity;
        roughness       = newAppearance.roughness;
        refractionIndex = newAppearance.refractionIndex;
        reflectivity    = newAppearance.reflectivity;
    }
};



const Appearance GLASS = {
    glm::vec3(0, 0, 1), // color
    0.95,               // opacity
    0.1,                // roughness
    1.53,               // refraction index
    0.2,                // reflectivity
    false,              // useTextureMap
};

const Appearance CHROME = {
    glm::vec3(0.25, 0.25, 0.25), // color
    0.0,                         // opacity
    0.1,                         // roughness
    1.0,                         // refraction index
    1.0,                         // reflectivity
    false,                       // useTextureMap
};

const Appearance MATTE_RED = {
    glm::vec3(0.8, 0.1, 0.1), // color
    0.0,                      // opacity
    0.7,                      // roughness
    1.0,                      // refraction index
    0.0,                      // reflectivity
    false,                    // useTextureMap
};

const Appearance MATTE_GREEN = {
    glm::vec3(0.1, 0.8, 0.1), // color
    0.0,                      // opacity
    0.7,                      // roughness
    1.0,                      // refraction index
    0.0,                      // reflectivity
    false,                    // useTextureMap
};

const Appearance MATTE_BLUE = {
    glm::vec3(0.1, 0.1, 0.8), // color
    0.0,                      // opacity
    0.7,                      // roughness
    1.0,                      // refraction index
    0.0,                      // reflectivity
    false,                    // useTextureMap
};


const Appearance MATTE_WHITE = {
    glm::vec3(1, 1, 1), // color
    0.0,                // opacity
    10,                 // roughness
    1.0,                // refraction index
    0.0,                // reflectivity
    false,              // useTextureMap
};

const Appearance SHINY_WHITE = {
    glm::vec3(1, 1, 1), // color
    0.0,                // opacity
    0.0,                // roughness
    1.0,                // refraction index
    0.0,                // reflectivity
    false,              // useTextureMap
};

const Appearance TEXTURE = {
    glm::vec3(1, 1, 1), // color
    0.0,                // opacity
    0.0,                // roughness
    1.0,                // refraction index
    0.0,                // reflectivity
    true,               // useTextureMap
};


#endif