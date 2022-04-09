#pragma once

#include <GLFW/glfw3.h>

#include "classes/sceneNode.hpp"


// Init
void initScene(GLFWwindow *window);
void initSceneGraph();
// Update
void updateState(float deltaTime);
void updateEnvironmentBuffers();
// Render
void renderFrame();
void renderNode(SceneNode *node, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition);
