#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "classes/sceneNode.hpp"
#include "classes/shader.hpp"
#include "options.hpp"



// Very basic shader manager that contains all the available shaders (except the skybox one, which manages itself)
// and allows switching between them dynamically
class ShaderManager
{
public:
    Shader *reflectionShader;
    Shader *refractionShader;
    Shader *sunlightShader;


    ShaderManager()
    {
        reflectionShader     = new Shader("main.vert", "reflective.frag");
        refractionShader     = new Shader("main.vert", "refractive.frag");
        sunlightShader       = new Shader("main.vert", "sunlight.frag");
    }

    /**
     * @brief Finds the correct shader for the node, if node is not able to be rendered returns nullptr
     *
     * @param node The node that is going to be rendered next
     */
    Shader *getShaderFor(SceneNode *node)
    {
        if (node->appearance == REFLECTIVE) return reflectionShader;
        if (node->appearance == REFRACTIVE) return refractionShader;
        if (node->appearance == SUNLIT) return sunlightShader;
        return nullptr;
    }
};

#endif