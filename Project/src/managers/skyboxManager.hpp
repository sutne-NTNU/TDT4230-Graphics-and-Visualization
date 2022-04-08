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
    glm::mat4 V; // View Matrix
    glm::mat4 P; // Projection Matrix
    Shader *skyboxShader;

public:
    SkyboxManager(Shader *skyboxShader)
    {
        this->skyboxShader = skyboxShader;
        activeSkyboxIndex  = 0;
        if (OPTIONS::mode == OPTIONS::DEMO) activeSkyboxIndex = 2;

        skyboxes.push_back(Skybox(
            "forest", ".png",                         // Images
            glm::vec3(0.529871, -0.340380, 0.776775), // Sunlight Direction
            glm::vec3(0.98, 0.97, 0.78)               // Sunlight Color
            ));

        if (OPTIONS::mode == OPTIONS::DEBUG) return; // Dont load the other skyboxes in debug mode

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



    glm::vec3 getSunDirection()
    {
        return skyboxes[activeSkyboxIndex].sunlightDirection;
    }

    glm::vec3 getSunColor()
    {
        return skyboxes[activeSkyboxIndex].sunlightColor;
    }

    void swapSkybox()
    {
        activeSkyboxIndex = (activeSkyboxIndex + 1) % skyboxes.size();
    }

    // Removes the translation from the view matrix and save V + P for rendering later
    void updateMatrices(glm::mat4 view, glm::mat4 projection)
    {
        V = glm::mat4(glm::mat3(view));
        P = projection;
    }

    void render()
    {
        skyboxShader->activate();
        skyboxShader->setUniform(UNIFORMS::V, V);
        skyboxShader->setUniform(UNIFORMS::P, P);
        skyboxes[activeSkyboxIndex].render();
    }
};

#endif