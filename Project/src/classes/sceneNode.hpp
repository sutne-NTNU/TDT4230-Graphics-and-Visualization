#ifndef SCENENODE_HPP
#define SCENENODE_HPP
#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "structs/appearance_struct.hpp"
#include "structs/mesh_struct.hpp"
#include "utilities/mesh.hpp"
#include "utilities/utils.hpp"


struct VAO
{
    unsigned int ID         = -1;
    unsigned int indexCount = 0;
};

class SceneNode
{
public:
    // A list of all children that belong to this node.
    std::vector<SceneNode *> children;
    // The node's position relative to its parent
    glm::vec3 position;
    // The node's rotation relative to its parent
    glm::vec3 rotation;
    // Scale relative to its starting size
    glm::vec3 scale;
    // The location of the node's reference point
    glm::vec3 referencePoint;

    // Transformation matrix representing the transformation of the node's location
    glm::mat4 M;
    // The ModelViewProjection Matrix For this node
    glm::mat4 MVP;
    // The Normal (Transformation) Matrix
    glm::mat3 N;

    // The ID of the VAO containing the "appearance" of this SceneNode.
    VAO vao;

    // The appearance of this node, wether that is a single color, texture etc.
    Appearance appearance;



    SceneNode()
    {
        position       = glm::vec3(0, 0, 0);
        rotation       = glm::vec3(0, 0, 0);
        scale          = glm::vec3(1, 1, 1);
        referencePoint = glm::vec3(0, 0, 0);
    }



    /** Initializes a SceneNode with VAO and VAI index count from a mesh */
    static SceneNode *fromMesh(Mesh mesh, Appearance appearance)
    {
        SceneNode *node      = new SceneNode();
        node->vao.ID         = UTILS::generateBuffer(mesh);
        node->vao.indexCount = mesh.indices.size();
        node->appearance     = appearance;
        return node;
    }



    /**
     * Convenience function thats very specific to just my models as they are all downloaded from the same source: Poly Haven
     *
     * Create a model and textures it using the files:
     *      ../res/models/<name>/<name>_01_2k.gltf
     *      ..7res/models/<name>/textures/<name>_01_diff_2k.png
     *      ../res/models/<name>/textures/<name>_01_nor_gl_2k.png
     *      ../res/models/<name>/textures/<name>_01_rough_2k.png
     */
    static SceneNode *fromModelName(const std::string &name, const std::string &resolution)
    {
        std::string modelName     = name + "/" + name + "_01_" + resolution + ".gltf";
        std::string diffuseName   = name + "/textures/" + name + "_01_diff_" + resolution + ".png";
        std::string normalName    = name + "/textures/" + name + "_01_nor_gl_" + resolution + ".png";
        std::string roughnessName = name + "/textures/" + name + "_01_rough_" + resolution + ".png";

        Mesh mesh       = GLTF::loadMeshFrom(modelName);
        SceneNode *node = fromMesh(mesh, MATTE_WHITE);
        UTILS::addTextureMaps(&node->appearance, diffuseName, normalName, roughnessName);
        return node;
    }



    // Add a child node to its parent's list of children
    void addChild(SceneNode *child)
    {
        children.push_back(child);
    }

    // Rotation along the axis in Degrees
    void rotate(float x = 0, float y = 0, float z = 0)
    {
        rotation += glm::radians(glm::vec3(x, y, z));
    }

    void setScale(float newScale)
    {
        scale = glm::vec3(newScale);
    }

    // Translate along the axis
    void translate(float x = 0, float y = 0, float z = 0)
    {
        position.x += x;
        position.y += y;
        position.z += z;
    }

    // Recursively find total amount of children bewlow this node
    unsigned int getNumChildren()
    {
        unsigned int numChildren = children.size();
        for (SceneNode *child : children)
        {
            numChildren += child->getNumChildren();
        }
        return numChildren;
    }

    // Returns every scenenode bewlo this node in a vector
    std::vector<SceneNode *> getAllChildren()
    {
        std::vector<SceneNode *> allChildren;
        for (SceneNode *child : children)
        {
            allChildren.push_back(child);
            std::vector<SceneNode *> childChildren = child->getAllChildren();
            allChildren.insert(allChildren.end(), childChildren.begin(), childChildren.end());
        }
        return allChildren;
    }
};

#endif