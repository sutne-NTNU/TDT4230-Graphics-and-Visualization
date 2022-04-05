#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "image.hpp"
#include "mesh.hpp"
#include "structs/appearance_struct.hpp"
#include "window.hpp"



namespace UTILS
{
    /**
     * @brief Creates and returns view matrix based on the params
     *
     * @param position The psotion of the "camera"
     * @param direction Unit vector pointing in the direction the camera is looking
     * @param up Unit vector pointing in the direction of the "up" vector
     * @return glm::mat4 View Matrix
     */
    glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
    {
        return glm::lookAt(position, position + direction, up);
    }

    /**
     * @brief Creates and returns projection matrix based on the params
     *
     * @param FOV Field of View of the camera
     * @return glm::vec4
     */
    glm::mat4 getPerspectiveMatrix(int FOV)
    {
        return glm::perspective(glm::radians((float)FOV), float(WINDOW::width) / float(WINDOW::height), 0.1f, 350.f);
    }


    template <class T>
    unsigned int generateAttribute(int id, int elementsPerEntry, std::vector<T> data, bool normalize)
    {
        unsigned int bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(id, elementsPerEntry, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, sizeof(T), 0);
        glEnableVertexAttribArray(id);
        return bufferID;
    }


    /**
     * @brief Calculates Tangents and Bitangents
     *
     * Direct Copy From: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
     * But without the normals, as they weren't used
     *
     * @param vertices Vertices of Mesh
     * @param uvs Texture Coordinates
     * @param tangents (Output) where the tangents will be stored
     * @param bitangents (Outputs) where the bitangents will be stored
     */
    void computeTangentBasis(
        std::vector<glm::vec3> &vertices,
        std::vector<glm::vec2> &uvs,
        std::vector<unsigned int> &indices,
        std::vector<glm::vec3> &tangents,
        std::vector<glm::vec3> &bitangents)
    {
        for (int i = 0; (i + 2) < indices.size(); i += 3)
        {
            // Shortcuts for vertices
            glm::vec3 &v0 = vertices[indices[i + 0]];
            glm::vec3 &v1 = vertices[indices[i + 1]];
            glm::vec3 &v2 = vertices[indices[i + 2]];

            // Shortcuts for UVs
            glm::vec2 &uv0 = uvs[indices[i + 0]];
            glm::vec2 &uv1 = uvs[indices[i + 1]];
            glm::vec2 &uv2 = uvs[indices[i + 2]];

            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

            glm::vec3 tangent   = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r);
            glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r);

            for (int j = 0; j < 3; j++)
            {
                tangents.push_back(tangent);
                bitangents.push_back(bitangent);
            }
        }
    }


    unsigned int generateBuffer(Mesh &mesh)
    {
        unsigned int vaoID;
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        generateAttribute(0, 3, mesh.vertices, false);
        generateAttribute(1, 3, mesh.normals, true);
        if (mesh.textureCoordinates.size() > 0)
        {
            std::vector<glm::vec3> tangents, bitangents;
            computeTangentBasis(mesh.vertices, mesh.textureCoordinates, mesh.indices, tangents, bitangents);
            generateAttribute(2, 3, tangents, true);
            generateAttribute(3, 3, bitangents, true);
            generateAttribute(4, 2, mesh.textureCoordinates, false);
        }

        unsigned int indexBufferID;
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        return vaoID;
    }



    /**
     * @brief Initialize a texture and return its ID
     *
     * @param texture the loaded texture
     * @return unsigned int
     */
    unsigned int initTexture(Image texture)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }


    /**
     * @brief Change the appearance based on loaded texture files
     *
     * @param diffuse The filename of the diffuse texture.
     * @param normal The filename of the normal texture.
     * @param roughness The filename of the roughness texture.
     * @param root The root directory of models
     */
    void addTextureMaps(
        Appearance *appearance,
        std::string const &diffuse,
        std::string const &normal,
        std::string const &roughness,
        std::string const &root)
    {
        std::ifstream fdiff((root + diffuse).c_str());
        if (!fdiff.fail())
        {
            appearance->hasTexture      = true;
            appearance->useTextureMap   = true;
            appearance->texture.colorID = initTexture(IMAGE::loadFrom((root + diffuse)));
        }
        else
        {
            std::cout << "Could not load texture: " << (root + diffuse) << std::endl;
        }
        std::ifstream fnor((root + normal).c_str());
        if (!fnor.fail())
        {
            appearance->hasTexture       = true;
            appearance->texture.normalID = initTexture(IMAGE::loadFrom((root + normal)));
        }
        else
        {
            std::cout << "Could not load texture: " << (root + normal) << std::endl;
        }
        std::ifstream frough((root + roughness).c_str());
        if (!frough.fail())
        {
            appearance->hasTexture          = true;
            appearance->texture.roughnessID = initTexture(IMAGE::loadFrom((root + roughness)));
        }
        else
        {
            std::cout << "Could not load texture: " << (root + roughness) << std::endl;
        }
    }



    glm::vec3 sphereCoordinates(float radius, int slice, int slices, int layer, int layers)
    {
        // math from: http://www.songho.ca/opengl/gl_sphere
        float theta = 2 * PI * slice / slices;
        float phi   = PI * layer / layers - PI / 2;

        float x = radius * glm::cos(phi) * glm::cos(theta);
        float y = radius * glm::sin(phi);
        float z = radius * glm::cos(phi) * glm::sin(theta);

        return glm::vec3(x, y, z);
    }


    void takeScreenshot(GLFWwindow *window)
    {
        // Get pixel data from screen
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        uint8_t *pixels = (uint8_t *)malloc(width * height * 4);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Get current timestamp as string
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, 80, " %d-%m-%Y %H%M%S", timeinfo);
        std::string timestamp = std::string(buffer);

        // Save image to file
        IMAGE::write("screenshot" + timestamp + ".png", width, height, pixels);
    }
}