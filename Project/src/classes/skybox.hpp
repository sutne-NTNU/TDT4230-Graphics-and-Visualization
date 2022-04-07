#ifndef SKYBOX_HPP
#define SKYBOX_HPP
#pragma once


#include <iostream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <stb_image.h>

#include "image.hpp"
#include "options.hpp"
#include "shader.hpp"



class Skybox
{
public:
    unsigned int vaoID;
    unsigned int textureID;
    glm::vec3 sunlightDirection;
    glm::vec3 sunlightColor;
    glm::mat4 VP; // custom view projection matrix that removes the translation component of the view matrix

    /**
     * @brief loads a cubemap texture from 6 different images.
     *
     * To get such an image you can just find a panorama and use: https://jaxry.github.io/panorama-to-cubemap/
     * which will create the correct faces places horizontally.
     *
     * The name of the files must be as follows:
     *         |--------|
     *         |  top   |
     * |------------------------------|
     * | right | front  | left | back |
     * |------------------------------|
     *         | bottom |
     *         |--------|
     *
     * These are based on the following axis directions:
     *      |----|
     *      | +Y |
     * |-------------------|
     * | +X | +Z | -X | -Z |
     * |-------------------|
     *      | -Y |
     *      |----|
     *
     * @param name folder name with images
     * @param extension the extension of all the images (.jpg / .png)
     * @param sunlightDirection the direction of the sunlight in this skybox
     * @param sunlightColor the color of the sunlight in this skybox
     * @param root the root folder where cubemaps are stored
     */
    Skybox(std::string name,
           std::string extensions,
           glm::vec3 sunlightDirection,
           glm::vec3 sunlightColor,
           std::string root = "../res/cubemaps/")
    {
        std::vector<std::string> faces = {
            root + name + "/right" + extensions,  // +X
            root + name + "/left" + extensions,   // -X
            root + name + "/top" + extensions,    // +Y
            root + name + "/bottom" + extensions, // -Y
            root + name + "/front" + extensions,  // +Z
            root + name + "/back" + extensions,   // -Z
        };

        // Initialize VAO
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);
        // Intialize VBO
        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // Create Texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        // Load skybox faces to the texture
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            Image image = Image(faces[i]);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        this->sunlightDirection = sunlightDirection;
        this->sunlightColor     = sunlightColor;
    }


    // Removes the translation from the view matrix and save VP for drawing later
    void updateVP(glm::mat4 view, glm::mat4 projection)
    {
        VP = projection * glm::mat4(glm::mat3(view));
    }


    void render(Gloom::Shader *shader)
    {
        shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::SKYBOX);
        shader->setUniform(UNIFORMS::MVP, VP);
        glDepthMask(GL_FALSE);
        glBindVertexArray(vaoID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
    }



private:
    float vertices[6 * 6 * 3] = {
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };
};


bool operator==(const Skybox &box1, const Skybox &box2)
{
    return box1.textureID == box2.textureID;
}

#endif