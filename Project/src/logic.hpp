#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "classes/sceneNode.hpp"
#include "utilities/framebuffer.hpp"



void initScene(GLFWwindow *window);
void initSceneGraph();
void updateState(GLFWwindow *window, double deltaTime);
void updateNodeTransformations(SceneNode *node, glm::mat4 M, glm::mat4 VP);
void renderFrame(GLFWwindow *window);
void renderRefractionStep(GLFWwindow *window);
void renderReflectionStep(GLFWwindow *window);
void renderFinal(GLFWwindow *window);
void renderNode(SceneNode *node);

void renderContentsOf(FrameBuffer framebuffer);