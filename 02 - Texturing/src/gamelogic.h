#pragma once

#include "sceneGraph.hpp"
#include "utilities/imageLoader.hpp"
#include <utilities/window.hpp>

void initGame(GLFWwindow *window, CommandLineOptions options);
void initObjects();
unsigned int initTexture(PNGImage texture);
SceneNode *createTextNode(float x, float y, std::string text, unsigned int width);
void updateFrame(GLFWwindow *window);
void updateGameState(GLFWwindow *window);
void updateNodeTransformations(SceneNode *node, glm::mat4 M, glm::mat4 VP);
void renderFrame(GLFWwindow *window);
void renderNode(SceneNode *node);