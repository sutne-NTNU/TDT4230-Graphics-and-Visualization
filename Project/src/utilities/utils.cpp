#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "classes/image.hpp"
#include "classes/mesh.hpp"
#include "classes/sceneNode.hpp"
#include "options.hpp"



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
     * @param FOV Field of View of the camera in degrees
     */
    glm::mat4 getPerspectiveMatrix(float FOV)
    {
        return glm::perspective(glm::radians(FOV), float(WINDOW::width) / float(WINDOW::height), 0.1f, 350.f);
    }

    /**
     * @brief Finds x, y and z coordinates on the surface of a sphere where the sphere is divided
     * into slices (think cake slices) and layers.
     *
     * @param radius Radius of the sphere
     * @param slice Current slice
     * @param slices Total number of slices
     * @param layer Current layer
     * @param layers Total number of layers
     */
    glm::vec3 sphereCoordinates(float radius, int slice, int slices, int layer, int layers)
    {
        // math from: http://www.songho.ca/opengl/gl_sphere.html
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
        strftime(buffer, 80, "%d-%m-%Y %H%M%S", timeinfo);
        std::string timestamp = std::string(buffer);

        // Save image to file
        Image::write("screenshot " + timestamp + ".png", width, height, pixels);
    }

    void print(std::string desc, glm::vec3 vec)
    {
        std::cout << desc << ": (" << vec.x << ", " << vec.y << ", " << vec.z << ")" << std::endl;
    }

    // void showFrameBufferContent(Framebuffer frambeuffer)
    // {
    //     Mesh mesh = Mesh();

    //     mesh.addQuad(
    //         glm::vec3(-1.0f, -1.0f, 0.0f),
    //         glm::vec3(1.0f, -1.0f, 0.0f),
    //         glm::vec3(1.0f, 1.0f, 0.0f),
    //         glm::vec3(-1.0f, 1.0f, 0.0f));

    //     mesh.textureCoordinates = {
    //         glm::vec2(0.0f, 0.0f),
    //         glm::vec2(0.0f, 1.0f),
    //         glm::vec2(1.0f, 1.0f),
    //         glm::vec2(1.0f, 0.0f)
    //     };


    // }
}