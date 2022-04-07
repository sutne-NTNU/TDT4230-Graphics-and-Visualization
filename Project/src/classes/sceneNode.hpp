#ifndef SCENENODE_HPP
#define SCENENODE_HPP
#pragma once

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "classes/shader.hpp"
#include "framebuffer.hpp"
#include "mesh.hpp"
#include "structs/appearance_struct.hpp"
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

    // Framebuffer used to store the dynamic reflection cubemap for this specific node
    Framebuffer *reflectionBuffer;
    // Framebuffer used to store the dynamic refraction cubemap for this specific node
    Framebuffer *refractionBuffer;



    SceneNode()
    {
        position       = glm::vec3(0, 0, 0);
        rotation       = glm::vec3(0, 0, 0);
        scale          = glm::vec3(1, 1, 1);
        referencePoint = glm::vec3(0, 0, 0);

        reflectionBuffer = new Framebuffer(100);
        refractionBuffer = new Framebuffer(100);
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
     *      ../res/models/<name>/<name>_01_<resolution>.gltf
     *      ..7res/models/<name>/textures/<name>_01_diff_<resolution>.png
     *      ../res/models/<name>/textures/<name>_01_nor_gl_<resolution>.png
     *      ../res/models/<name>/textures/<name>_01_rough_<resolution>.png
     */
    static SceneNode *fromModelName(const std::string &name, const std::string &resolution)
    {
        std::string modelName     = name + "/" + name + "_01_" + resolution + ".gltf";
        std::string diffuseName   = name + "/textures/" + name + "_01_diff_" + resolution + ".png";
        std::string normalName    = name + "/textures/" + name + "_01_nor_gl_" + resolution + ".png";
        std::string roughnessName = name + "/textures/" + name + "_01_rough_" + resolution + ".png";

        Mesh mesh       = Mesh(modelName);
        SceneNode *node = fromMesh(mesh, MATTE_WHITE);
        UTILS::addTextureMaps(&node->appearance, diffuseName, normalName, roughnessName);
        return node;
    }



    /**
     * @brief Updates all transformations for node and recursively for all its children
     *
     * @param node
     * @param M Model Transformation Matrix
     * @param VP ViewProjection Transformation Matrix
     */
    void updateTransformations(glm::mat4 M, glm::mat4 VP)
    {
        glm::mat4 myTransformation = glm::translate(position)
                                   * glm::translate(referencePoint)
                                   * glm::rotate(rotation.y, glm::vec3(0, 1, 0))
                                   * glm::rotate(rotation.x, glm::vec3(1, 0, 0))
                                   * glm::rotate(rotation.z, glm::vec3(0, 0, 1))
                                   * glm::scale(scale)
                                   * glm::translate(-referencePoint);

        this->M   = M * myTransformation;
        this->N   = glm::mat3(glm::transpose(glm::inverse(this->M)));
        this->MVP = VP * this->M;

        // Update children
        for (SceneNode *child : children) child->updateTransformations(this->M, VP);
    }



    /** Recursively Render this node with its children */
    void render(Gloom::Shader *shader)
    {
        bool isRenderable = vao.ID != -1;
        if (isRenderable)
        {
            // Pass transformation matrices to vertex shader
            shader->setUniform(UNIFORMS::M, M);
            shader->setUniform(UNIFORMS::MVP, MVP);
            shader->setUniform(UNIFORMS::N, N);

            // Pass material information to fragment shader
            shader->setUniform("appearance.color", appearance.color);
            shader->setUniform("appearance.opacity", appearance.opacity);
            shader->setUniform("appearance.roughness", appearance.roughness);
            shader->setUniform("appearance.reflectivity", appearance.reflectivity);
            shader->setUniform("appearance.refraction_index", appearance.refractionIndex);

            if (!appearance.hasTexture)
            {
                shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::GEOMETRY_SHAPE);
            }
            else
            {
                shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::GEOMETRY_TEXTURED);
                shader->setUniform("appearance.use_texture_map", appearance.useTextureMap);

                glBindTextureUnit(BINDINGS::texture_map, appearance.texture.colorID);
                glBindTextureUnit(BINDINGS::normal_map, appearance.texture.normalID);
                glBindTextureUnit(BINDINGS::roughness_map, appearance.texture.roughnessID);
            }
            // Bind this nodes vertices and draw
            glBindVertexArray(vao.ID);
            glDrawElements(GL_TRIANGLES, vao.indexCount, GL_UNSIGNED_INT, nullptr);
        }
        // Render children
        for (SceneNode *child : children) child->render(shader);
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

    // Returns every scenenode below this node in the scenegraph in a vector
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