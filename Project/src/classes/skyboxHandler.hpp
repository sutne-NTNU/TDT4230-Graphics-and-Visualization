#ifndef SKYBOX_HPP
#define SKYBOX_HPP
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "options.hpp"
#include "utilities/cubemap.hpp"



struct Skybox
{
    unsigned int id;
    glm::vec3 sunlightDirection;
    glm::vec3 sunlightColor;

    Skybox(unsigned int id, glm::vec3 sunlightDirection, glm::vec3 sunlightColor)
    {
        this->id                = id;
        this->sunlightDirection = sunlightDirection;
        this->sunlightColor     = sunlightColor;
    }
};

bool operator==(const Skybox &box1, const Skybox &box2)
{
    return box1.id == box2.id;
}



class SkyboxHandler
{
private:
    int activeSkyboxIndex;
    std::vector<Skybox> skyboxes;

public:
    unsigned int vaoID;
    glm::mat4 VP;

    SkyboxHandler(int startIndex = 0)
    {
        // all cubemaps use the same vertices, and hence can use the same vao
        vaoID             = CUBEMAP::generateBuffer();
        activeSkyboxIndex = startIndex;

        skyboxes.push_back(
            Skybox(
                CUBEMAP::load("lake", ".jpg"),            // Images
                glm::vec3(0.422755, -0.338738, 0.840556), // Sunlight Direction
                glm::vec3(0.98, 0.96, 1)                  // Sunlight Color
                ));

        if (OPTIONS::mode == OPTIONS::DEBUG)
        {
            activeSkyboxIndex = 0;
            return;
        }

        skyboxes.push_back(
            Skybox(
                CUBEMAP::load("forest", ".png"),          // Images
                glm::vec3(0.529871, -0.340380, 0.776775), // Sunlight Direction
                glm::vec3(0.98, 0.97, 0.78)               // Sunlight Color
                ));

        skyboxes.push_back(
            Skybox(
                CUBEMAP::load("kiara_dawn", ".png"),      // Images
                glm::vec3(0.811961, -0.101056, 0.574898), // Sunlight Direction
                glm::vec3(156, 121, 131) / 255.0          // Sunlight Color
                ));

        skyboxes.push_back(
            Skybox(
                CUBEMAP::load("forest2", ".png"),         // Images
                glm::vec3(0.539989, -0.414694, 0.732421), // Sunlight Direction
                glm::vec3(1, 1, 1)                        // Sunlight Color
                ));
    }

    // Removes the translation from the view matrix and save VP for drawing later
    void updateVP(glm::mat4 view, glm::mat4 projection)
    {
        VP = projection * glm::mat4(glm::mat3(view));
    }

    GLint getTextureID()
    {
        return skyboxes[activeSkyboxIndex].id;
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
};

#endif