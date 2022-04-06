#include "cubemap.hpp"

#include <iostream>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <stb_image.h>

#include "options.hpp"
#include "utilities/image.hpp"



namespace CUBEMAP
{

    unsigned int generateBuffer()
    {
        unsigned int vao, vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        return vao;
    }



    /**
     * @brief loads a cubemap texture from 6 different images.
     *
     * To get such an image you can just find a panorama and use: https://jaxry.github.io/panorama-to-cubemap/
     * which will create the correct faces places horixzontally, hence why i rotate the image
     * in this method because the data is read from the pixels row-wise.
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
     * These are based on the following axis directio (that the images ususally are stored in):
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
     * @return Texture ID
     */
    unsigned int load(std::string name, std::string extension, std::string root)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        std::vector<std::string> faces = {
            root + name + "/right" + extension,  // +X
            root + name + "/left" + extension,   // -X
            root + name + "/top" + extension,    // +Y
            root + name + "/bottom" + extension, // -Y
            root + name + "/front" + extension,  // +Z
            root + name + "/back" + extension,   // -Z
        };
        if (extension == ".jpg")
        {
            // Struggling to get .jpg to work properly for some reason, so handle it in this nasty way
            int width, height, channels;
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);
                if (data)
                {
                    glTexImage2D(sides[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                    if (OPTIONS::verbose) std::cout << "Loaded image: " << faces[i] << " \tWidth: " << width << " Height: " << height << " Channels: " << channels << std::endl;
                }
                else
                    std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        else
        {
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                Image image = IMAGE::loadFrom(faces[i]);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return textureID;
    }
}
