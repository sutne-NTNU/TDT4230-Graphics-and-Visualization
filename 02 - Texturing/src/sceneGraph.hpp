#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <stack>
#include <stdbool.h>
#include <vector>

enum SceneNodeType
{
    GEOMETRY_2D,
    GEOMETRY_3D,
    GEOMETRY_TEXTURED,
    POINT_LIGHT,
    SPOT_LIGHT,
};

struct SceneNode
{
    SceneNode()
    {
        position           = glm::vec3(0, 0, 0);
        rotation           = glm::vec3(0, 0, 0);
        scale              = glm::vec3(1, 1, 1);
        referencePoint     = glm::vec3(0, 0, 0);
        type               = GEOMETRY_3D;
        vaoIndexCount      = 0;
        vaoID              = -1;
        lightID            = -1;
        textureID          = -1;
        textureIDNormal    = -1;
        textureIDRoughness = -1;
    }

    // A list of all children that belong to this node.
    // For instance, in case of the scene graph of a human body shown in the assignment text, the "Upper Torso" node would contain the "Left Arm", "Right Arm", "Head" and "Lower Torso" nodes in its list of children.
    std::vector<SceneNode *> children;
    // The node's position relative to its parent
    glm::vec3 position;
    // The node's rotation relative to its parent
    glm::vec3 rotation;
    // Scale relative to its starting size
    glm::vec3 scale;
    // Transformation matrix representing the transformation of the node's location
    glm::vec3 referencePoint;
    // The ID of the VAO containing the "appearance" of this SceneNode.
    glm::mat4 M;
    // The ModelViewProjection Matrix For this node
    glm::mat4 MVP;
    // The Normal (Transformation) Matrix
    glm::mat3 N;
    // The location of the node's reference point
    int vaoID;
    // Number of indices for mesh
    unsigned int vaoIndexCount;
    // Node type is used to determine how to handle the contents of a node
    SceneNodeType type;

    // LightId (used as index in array of lights)
    int lightID;
    // Color of the light
    glm::vec3 lightColor;
    // World Light Position
    glm::vec3 lightPosition;

    // Texture ID
    unsigned int textureID;
    // Normal Mapped Texture ID
    unsigned int textureIDNormal;
    // Normal Mapped Texture ID
    unsigned int textureIDRoughness;
};

SceneNode *createSceneNode();
SceneNode *createLightNode(SceneNodeType type);
void addChild(SceneNode *parent, SceneNode *child);
void printNode(SceneNode *node);
int totalChildren(SceneNode *parent);
