#pragma once

#include <GLFW/glfw3.h>

#include "classes/sceneNode.hpp"



void initScene(GLFWwindow *window);
void initSceneGraph();
void updateState(float deltaTime);
void renderFrame();
void fillReflectionBuffer(SceneNode *node);
void updateBackSideNormals();

enum SpecialRenderingCase
{
    NONE,
    SKIP,         // Dont render
    ONLY_BACKSIDE // Change to front-face culling
};

void renderNode(SceneNode *node, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, bool flipNormals = false);
void renderSceneGraph(glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, SceneNode *specialNode = nullptr, SpecialRenderingCase specialCase = NONE);

void demoRender();