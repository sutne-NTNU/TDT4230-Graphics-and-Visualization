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

public:
    SkyboxManager()
    {
        activeSkyboxIndex = 0;
        if (OPTIONS::mode == OPTIONS::DEMO) activeSkyboxIndex = 2;

        skyboxes.push_back(Skybox(
            "lake", ".jpg",
            glm::vec3(0.422755, -0.338738, 0.840556), // Sunlight Direction
            glm::vec3(0.98, 0.96, 1)                  // Sunlight Color
            ));

        if (OPTIONS::mode == OPTIONS::DEBUG) return; // Dont load the other skyboxes in debug mode

        skyboxes.push_back(Skybox(
            "forest", ".png",                         // Images
            glm::vec3(0.529871, -0.340380, 0.776775), // Sunlight Direction
            glm::vec3(0.98, 0.97, 0.78)               // Sunlight Color
            ));

        skyboxes.push_back(Skybox(
            "kiara_dawn", ".png",                     // Images
            glm::vec3(0.811961, -0.101056, 0.574898), // Sunlight Direction
            glm::vec3(156, 121, 131) / 255.0f         // Sunlight Color
            ));

        skyboxes.push_back(Skybox(
            "forest2", ".png",                        // Images
            glm::vec3(0.539989, -0.414694, 0.732421), // Sunlight Direction
            glm::vec3(1, 1, 1)                        // Sunlight Color
            ));
    }

    // Removes the translation from the view matrix and save VP for drawing later
    void updateVP(glm::mat4 view, glm::mat4 projection)
    {
        skyboxes[activeSkyboxIndex].updateVP(view, projection);
    }

    glm::vec3 getSunDirection()
    {
        return skyboxes[activeSkyboxIndex].sunlightDirection;
    }

    glm::vec3 getSunColor()
    {
        return skyboxes[activeSkyboxIndex].sunlightColor;
    }

    void swapCubemap()
    {
        activeSkyboxIndex = (activeSkyboxIndex + 1) % skyboxes.size();
    }

    void render(Gloom::Shader *shader)
    {
        skyboxes[activeSkyboxIndex].render(shader);
    }
};

#endif