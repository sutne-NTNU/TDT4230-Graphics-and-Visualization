#ifndef SKYBOX_MANAGER_HPP
#define SKYBOX_MANAGER_HPP
#pragma once


#include "classes/shader.hpp"
#include "classes/skybox.hpp"
#include "options.hpp"


// Keeps track of all the loaded skyboxes and allows switching between them dynamically
class SkyboxManager
{
private:
    int activeSkyboxIndex;
    std::vector<Skybox> skyboxes;
    Shader *skyboxShader;

public:
    SkyboxManager()
    {
        skyboxShader      = new Shader("skybox.vert", "skybox.frag");
        activeSkyboxIndex = 0;

        skyboxes.push_back(Skybox(
            "mountain", ".png",                       // Images
            glm::vec3(0.529871, -0.340380, 0.776775), // Sunlight Direction
            glm::vec3(0.98, 0.97, 0.88)               // Sunlight Color
            ));

        if (OPTIONS::mode == OPTIONS::DEBUG) return; // Dont load the other skyboxes in debug mode

        skyboxes.push_back(Skybox(
            "kiara_dawn", ".png",                     // Images
            glm::vec3(0.811961, -0.101056, 0.574898), // Sunlight Direction
            glm::vec3(156, 121, 131) / 255.0f         // Sunlight Color
            ));

        skyboxes.push_back(Skybox(
            "forest", ".png",                         // Images
            glm::vec3(0.539989, -0.414694, 0.732421), // Sunlight Direction
            glm::vec3(1, 1, 1)                        // Sunlight Color
            ));

        skyboxes.push_back(Skybox(
            "lake", ".jpg",                           // Images
            glm::vec3(0.422755, -0.338738, 0.840556), // Sunlight Direction
            glm::vec3(0.98, 0.96, 1)                  // Sunlight Color
            ));
    }



    glm::vec3 getSunlightDirection()
    {
        return skyboxes[activeSkyboxIndex].sunlightDirection;
    }

    glm::vec3 getSunlightColor()
    {
        return skyboxes[activeSkyboxIndex].sunlightColor;
    }

    unsigned int getTextureID()
    {
        return skyboxes[activeSkyboxIndex].textureID;
    }

    void swapSkybox()
    {
        activeSkyboxIndex = (activeSkyboxIndex + 1) % skyboxes.size();
    }

    /**
     * @brief Renders the current skybox using the given view and projection matrices
     *
     * @param view View Matrix
     * @param projection Projection Matrix
     */
    void render(glm::mat4 view, glm::mat4 projection)
    {
        skyboxShader->activate();
        skyboxShader->setUniform(UNIFORMS::V, glm::mat4(glm::mat3(view))); // Remove translation
        skyboxShader->setUniform(UNIFORMS::P, projection);
        skyboxes[activeSkyboxIndex].render();
    }
};

#endif