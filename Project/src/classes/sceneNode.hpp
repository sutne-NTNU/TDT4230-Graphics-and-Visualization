#ifndef SCENENODE_HPP
#define SCENENODE_HPP
#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "classes/image.hpp"
#include "classes/shader.hpp"
#include "framebuffer.hpp"
#include "mesh.hpp"



// Im keeping it simple for myself
enum AppearanceType
{
    REFLECTIVE, // reflects 100% of light
    REFRACTIVE, // refracts with fresnel effect
    SUNLIT      // use color / diffuse map for color and add sunlight and shadows
};

// Keeps track of all three texture types im using
struct Textures
{
    bool hasTextures = false;
    int diffuseID    = -1;
    int normalID     = -1;
    int roughnessID  = -1;
};

struct VAO
{
    int ID         = -1;
    int indexCount = -1;
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

    // Transformation matrix representing the transformation of the node's location/rotation/scale
    glm::mat4 M;
    // The Normal (Transformation) Matrix for this nodes normals
    glm::mat3 N;

    // Information about vertices for this node
    VAO vao;

    // Framebuffer used to store the dynamic environment cubemap for this specific node
    Framebuffer *environmentBuffer;
    bool hasEnvironmentMap = false;

    // How the node should be render
    AppearanceType appearance;
    // Information about relevant textures (and if it has any)
    Textures textures;



    SceneNode()
    {
        position       = glm::vec3(0, 0, 0);
        rotation       = glm::vec3(0, 0, 0);
        scale          = glm::vec3(1, 1, 1);
        referencePoint = glm::vec3(0, 0, 0);

        environmentBuffer = new Framebuffer(OPTIONS::environmentBufferResolution);
    }



    /** Initializes a SceneNode with VAO and VAI index count from a mesh */
    static SceneNode *fromMesh(Mesh mesh, AppearanceType appearance)
    {
        SceneNode *node      = new SceneNode();
        node->vao.ID         = generateBuffer(mesh);
        node->vao.indexCount = (unsigned int)mesh.indices.size();
        node->appearance     = appearance;
        if (OPTIONS::verbose) printf("Created SceneNode with: %d indices, %d vertices\n", node->vao.indexCount, mesh.vertices.size());
        return node;
    }



    /**
     * Convenience function thats very specific to just my model as it downloaded from Poly Haven
     *
     * Create a model and textures it using the files:
     *      ../res/models/<name>/<name>_01_<resolution>.gltf
     *      ../res/models/<name>/textures/<name>_01_diff_<resolution>.png
     *      ../res/models/<name>/textures/<name>_01_nor_gl_<resolution>.png
     *      ../res/models/<name>/textures/<name>_01_rough_<resolution>.png
     */
    static SceneNode *fromModelName(const std::string &name, const std::string &resolution, AppearanceType appearance)
    {
        std::string modelName     = name + "/" + name + "_01_" + resolution + ".gltf";
        std::string diffuseName   = name + "/textures/" + name + "_01_diff_" + resolution + ".png";
        std::string normalName    = name + "/textures/" + name + "_01_nor_gl_" + resolution + ".png";
        std::string roughnessName = name + "/textures/" + name + "_01_rough_" + resolution + ".png";

        Mesh mesh       = Mesh(modelName);
        SceneNode *node = fromMesh(mesh, appearance);
        addTextureMaps(node, diffuseName, normalName, roughnessName);
        return node;
    }


    void increaseEnvironmentResolution()
    {
        int newResolution = environmentBuffer->width * 2;
        if (2048 < newResolution) return;
        environmentBuffer = new Framebuffer(newResolution);
        hasEnvironmentMap = false;
    }

    void decreaseEnvironmentResolution()
    {
        int newResolution = environmentBuffer->width / 2;
        if (newResolution < 32) return;
        environmentBuffer = new Framebuffer(newResolution);
        hasEnvironmentMap = false;
    }


    /**
     * @brief Updates all transformations for node and recursively for all its children
     *
     * @param parentModelMatrix Model Transformation Matrix of this nodes parent
     */
    void updateTransformations(glm::mat4 parentModelMatrix)
    {
        glm::mat4 myTransformation = glm::translate(position)
                                   * glm::translate(referencePoint)
                                   * glm::rotate(rotation.y, glm::vec3(0, 1, 0))
                                   * glm::rotate(rotation.x, glm::vec3(1, 0, 0))
                                   * glm::rotate(rotation.z, glm::vec3(0, 0, 1))
                                   * glm::scale(scale)
                                   * glm::translate(-referencePoint);

        M = parentModelMatrix * myTransformation;
        N = glm::mat3(glm::transpose(glm::inverse(M)));

        // Update children
        for (SceneNode *child : children) child->updateTransformations(M);
    }


    /**
     * @brief Render the given node with the given view, projection and camera position.
     * The method sets all uniform values in the given shader and renders the node's mesh.
     *
     * To make things a easy for myself im ditching the recursive rendering of the scenenodes.
     * This enables me to render the entire scene, but treat a specific node in a particular way (e.g skip it)
     * I have to do this to prevent the node trying to sample itself when creating the environment map, which creates ugly artifacts.
     * The ideal way to solve this would have the node be able to reflect itself, but i dont have time figure that out :/
     *
     * @param shader Which shader to use for rendering
     */
    void render(Shader *shader)
    {
        if (vao.ID == -1 || vao.indexCount <= 0) return;

        shader->setUniform(UNIFORMS::M, M);
        shader->setUniform(UNIFORMS::N, N);

        // let the shader know if it should use textures or not
        shader->setUniform(UNIFORMS::has_textures, textures.hasTextures);
        if (textures.hasTextures)
        {
            glBindTextureUnit(BINDINGS::diffuse_map, textures.diffuseID);
            glBindTextureUnit(BINDINGS::normal_map, textures.normalID);
            glBindTextureUnit(BINDINGS::roughness_map, textures.roughnessID);
        }

        // If node has environment map, replace the regular skybox
        if (hasEnvironmentMap) glBindTextureUnit(BINDINGS::skybox, environmentBuffer->textureID);

        // Finally render the nodes mesh
        glBindVertexArray(vao.ID);
        glDrawElements(GL_TRIANGLES, vao.indexCount, GL_UNSIGNED_INT, nullptr);
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
        size_t numChildren = children.size();
        for (SceneNode *child : children) numChildren += child->getNumChildren();
        return (unsigned int)numChildren;
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



    // Increase the enum value for appearance, and return to first type if it reaches the end
    void swapAppearance()
    {
        appearance = static_cast<AppearanceType>((appearance + 1) % (SUNLIT + 1));
    }



private:
    /**
     * @brief Creates the VAO and VBO for this node using a mesh, if mesh contains texture coordinates then it
     * computes the tangents and bitangents as well. Sends all mesh info to vertex shader.
     *
     * @param mesh
     * @return VAO ID
     */
    static unsigned int generateBuffer(Mesh &mesh)
    {
        // Create VAO
        unsigned int vaoID;
        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        // create Index Buffer
        unsigned int indexBufferID;
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        // Compute tangents and bitangents
        std::vector<glm::vec3> tangents, bitangents;
        if (mesh.textureCoordinates.size() > 0)
        {
            computeTangentBasis(mesh.vertices, mesh.textureCoordinates, mesh.indices, tangents, bitangents);
        }

        // Pass all info to the vertex shader
        generateAttribute(0, 3, mesh.vertices);
        generateAttribute(1, 3, mesh.normals, true);
        generateAttribute(2, 3, tangents, true);
        generateAttribute(3, 3, bitangents, true);
        generateAttribute(4, 2, mesh.textureCoordinates);

        return vaoID;
    }



    /**
     * @brief Sends the data as an attribute to the vertex shader at location "id"
     *
     * @tparam T
     * @param id location in the vertex shader
     * @param elementsPerEntry number of elements per entry
     * @param data data to send
     * @param normalize Wether to normalize the data or not
     * @return VAO id
     */
    template <class T>
    static void generateAttribute(int id, int elementsPerEntry, std::vector<T> data, bool normalize = false)
    {
        unsigned int bufferID;
        glGenBuffers(1, &bufferID);
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(id, elementsPerEntry, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, sizeof(T), 0);
        glEnableVertexAttribArray(id);
    }



    /**
     * @brief Initialize a texture with data from the image and return its texture ID
     *
     * @param texture the loaded image
     */
    static unsigned int initTexture(Image texture)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.pixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }



    /**
     * @brief Calculates Tangents and Bitangents
     *
     * Based on: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
     *
     * @param vertices Vertices of Mesh
     * @param uvs Texture Coordinates of mesh
     * @param indices Indices of Mesh
     * @param tangents (Output) where the tangents will be stored
     * @param bitangents (Outputs) where the bitangents will be stored
     */
    static void computeTangentBasis(
        std::vector<glm::vec3> &vertices,
        std::vector<glm::vec2> &uvs,
        std::vector<unsigned int> &indices,
        std::vector<glm::vec3> &tangents,
        std::vector<glm::vec3> &bitangents)
    {
        for (int i = 0; (i + 2) < indices.size(); i += 3)
        {
            // Shortcuts for vertices
            glm::vec3 &v0 = vertices[indices[i + 0]];
            glm::vec3 &v1 = vertices[indices[i + 1]];
            glm::vec3 &v2 = vertices[indices[i + 2]];

            // Shortcuts for UVs
            glm::vec2 &uv0 = uvs[indices[i + 0]];
            glm::vec2 &uv1 = uvs[indices[i + 1]];
            glm::vec2 &uv2 = uvs[indices[i + 2]];

            // Edges of the triangle : position delta
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;

            // UV delta
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

            glm::vec3 tangent   = glm::normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r);
            glm::vec3 bitangent = glm::normalize((deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r);

            for (int j = 0; j < 3; j++)
            {
                tangents.push_back(tangent);
                bitangents.push_back(bitangent);
            }
        }
    }



    /**
     * @brief Load and store textures to the given SceneNode
     *
     * @param diffuse The filename of the diffuse texture.
     * @param normal The filename of the normal texture.
     * @param roughness The filename of the roughness texture.
     * @param root The root directory of models
     */
    static void addTextureMaps(
        SceneNode *node,
        std::string const &diffuse,
        std::string const &normal,
        std::string const &roughness,
        std::string const &root = "../res/models/")
    {
        std::ifstream fdiff((root + diffuse).c_str());
        if (!fdiff.fail())
        {
            node->textures.diffuseID = initTexture(Image(root + diffuse));
        }
        else
        {
            printf("Could not load texture: %s\n", (root + diffuse).c_str());
            return;
        }
        std::ifstream fnor((root + normal).c_str());
        if (!fnor.fail())
        {
            node->textures.normalID = initTexture(Image((root + normal)));
        }
        else
        {
            printf("Could not load texture: %s\n", (root + normal).c_str());
            return;
        }
        std::ifstream frough((root + roughness).c_str());
        if (!frough.fail())
        {
            node->textures.roughnessID = initTexture(Image((root + roughness)));
        }
        else
        {
            printf("Could not load texture: %s\n", (root + roughness).c_str());
            return;
        }
        node->textures.hasTextures = true;
    }
};



#endif