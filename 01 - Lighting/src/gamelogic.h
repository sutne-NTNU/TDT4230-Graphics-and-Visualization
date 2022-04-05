#pragma once

#include "sceneGraph.hpp"
#include <utilities/window.hpp>

void initGame(GLFWwindow *window, CommandLineOptions options);
void initObjects();
void updateFrame(GLFWwindow *window);
void updateGameState(GLFWwindow *window);
void updateNodeTransformations(SceneNode *node, glm::mat4 M, glm::mat4 VP);
void renderFrame(GLFWwindow *window);
void renderNode(SceneNode *node);