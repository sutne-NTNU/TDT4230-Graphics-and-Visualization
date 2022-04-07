#pragma once

#include <string>

#include <glm/glm.hpp>

#include "structs/appearance_struct.hpp"
#include "structs/image_struct.hpp"
#include "structs/mesh_struct.hpp"


const float PI = 3.14159265359f;

namespace UTILS
{ 
    glm::mat4 getViewMatrix(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0, 1, 0));
    glm::mat4 getPerspectiveMatrix(float FOV);
    unsigned int generateBuffer(Mesh &mesh);
    unsigned int initTexture(Image texture);
    void addTextureMaps(Appearance *appearance,
                        std::string const &diffuse,
                        std::string const &normal,
                        std::string const &roughness,
                        std::string const &root = "../res/models/");
    glm::vec3 sphereCoordinates(float radius, int slice, int slices, int layer, int layers);
    void takeScreenshot(GLFWwindow *window);
    template <class T>
    unsigned int generateAttribute(int id, int elementsPerEntry, std::vector<T> data, bool normalize);
}