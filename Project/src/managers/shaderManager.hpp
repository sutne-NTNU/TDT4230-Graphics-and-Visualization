#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "classes/sceneNode.hpp"
#include "classes/shader.hpp"
#include "options.hpp"



// Very basic shader manager that contains all the available shaders (except the skybox one, that manages itself)
// and allows switching between them dynamically
class ShaderManager
{
private:
    Shader *reflectionShader;
    Shader *refractionShader;
    Shader *sunlightShader;

public:
    ShaderManager()
    {
        reflectionShader = new Shader("main.vert", "reflective.frag");
        refractionShader = new Shader("main.vert", "refractive.frag");
        sunlightShader   = new Shader("main.vert", "sunlight.frag");
    }

    /**
     * @brief Finds the correct shader for the node, checks if the shader is already active and if not, activates it
     *
     * @param node The node that is going to be rendered next
     */
    Shader *getShaderFor(SceneNode *node)
    {
        Shader *shader;
        switch (node->appearance)
        {
            case REFLECTIVE:
                shader = reflectionShader;
                break;
            case REFRACTIVE:
                shader = refractionShader;
                break;
            case CLASSIC:
                shader = sunlightShader;
                break;
            case TEXTURED:
                shader = sunlightShader;
                break;
            default:
                return nullptr;
        }
        // Check if this shader is already bound and activate it if it isnt
        GLint prog = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
        if (prog != shader->getProgram()) shader->activate();
        return shader;
    }
};

#endif