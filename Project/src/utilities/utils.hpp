#pragma once

#include <string>

#include <glm/glm.hpp>

#include "classes/image.hpp"
#include "classes/mesh.hpp"



const float PI = 3.14159265359f;

namespace UTILS
{
    glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
    glm::mat4 getPerspectiveMatrix(float FOV);
    glm::vec3 sphereCoordinates(float radius, int slice, int slices, int layer, int layers);
    void takeScreenshot(GLFWwindow *window);
}