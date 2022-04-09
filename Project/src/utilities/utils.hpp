#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



const float PI = 3.14159265359f;

namespace UTILS
{
    glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
    glm::mat4 getPerspectiveMatrix(float FOV, float aspectRatio);
    glm::vec3 sphereCoordinates(float radius, int slice, int slices, int layer, int layers);
    void takeScreenshot(GLFWwindow *window);
    void print(std::string desc, glm::vec3 vec);
}