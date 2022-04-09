#pragma once

#include <GLFW/glfw3.h>

#include "classes/sceneNode.hpp"
#include "classes/shader.hpp"



void initScene(GLFWwindow *window);
void initSceneGraph();
void updateState(float deltaTime);
void updateEnvironmentBuffers();
void renderFrame();
void renderNode(SceneNode *node, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, Shader *shader);
