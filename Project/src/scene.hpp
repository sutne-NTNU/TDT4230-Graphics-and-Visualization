#pragma once

#include <GLFW/glfw3.h>

#include "classes/sceneNode.hpp"



void initScene(GLFWwindow *window);
void initSceneGraph();
void updateState(GLFWwindow *window, float deltaTime);
void renderFrame(GLFWwindow *window);
void renderRefractionStep(GLFWwindow *window);
void renderReflectionStep(GLFWwindow *window);
void renderFinal(GLFWwindow *window);
