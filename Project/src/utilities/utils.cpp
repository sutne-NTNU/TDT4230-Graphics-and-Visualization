#include "utils.hpp"

#include <fstream>
#include <vector>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/common.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/transform.hpp>



#include "classes/image.hpp"
// #include "classes/mesh.hpp"
// #include "classes/sceneNode.hpp"
// #include "options.hpp"



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
     * @param FOV Field of View in degrees
     * @param aspectRatio Aspect ratio of the screen (Width / Height)
     */
    glm::mat4 getPerspectiveMatrix(float FOV, float aspectRatio)
    {
        return glm::perspective(glm::radians(FOV), aspectRatio, OPTIONS::nearClippingPlane, OPTIONS::farClippingPlane);
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


    /**
     * @brief Saves content of the current window framebuffer to "screenshot dd-mm-yyyy hhmmss.png"
     *
     * @param window The window to take a screenshot of
     */
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
        printf("%s: (%f, %f, %f)\n", desc.c_str(), vec.x, vec.y, vec.z);
    }
}