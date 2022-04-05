#include "glutils.h"
#include <glad/glad.h>
#include <program.hpp>
#include <vector>

#define OUT

template <class T>
unsigned int generateAttribute(int id, int elementsPerEntry, std::vector<T> data, bool normalize)
{
    unsigned int bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(id, elementsPerEntry, GL_FLOAT, normalize ? GL_TRUE : GL_FALSE, sizeof(T), 0);
    glEnableVertexAttribArray(id);
    return bufferID;
}

/**
 * @brief Calculates Tangents and Bitangents
 *
 * Direct Copy From: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
 * But without the normals, as they weren't used
 *
 * @param vertices Vertices of Mesh
 * @param uvs Texture Coordinates
 * @param tangents (Output) where the tangents will be stored
 * @param bitangents (Outputs) where the bitangents will be stored
 */
void computeTangentBasis(
    std::vector<glm::vec3> &vertices,
    std::vector<glm::vec2> &uvs,
    OUT std::vector<glm::vec3> &tangents,
    OUT std::vector<glm::vec3> &bitangents)
{
    for (int i = 0; (i + 2) < vertices.size(); i += 3)
    {
        // Shortcuts for vertices
        glm::vec3 &v0 = vertices[i + 0];
        glm::vec3 &v1 = vertices[i + 1];
        glm::vec3 &v2 = vertices[i + 2];

        // Shortcuts for UVs
        glm::vec2 &uv0 = uvs[i + 0];
        glm::vec2 &uv1 = uvs[i + 1];
        glm::vec2 &uv2 = uvs[i + 2];

        // Edges of the triangle : position delta
        glm::vec3 deltaPos1 = v1 - v0;
        glm::vec3 deltaPos2 = v2 - v0;

        // UV delta
        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        glm::vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        // Set the same tangent for all three vertices of the triangle.
        // They will be merged later, in vboindexer.cpp
        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        // Same thing for bitangents
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }
}


unsigned int generateBuffer(Mesh &mesh)
{
    unsigned int vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    generateAttribute(0, 3, mesh.vertices, false);
    generateAttribute(1, 3, mesh.normals, true);
    if (mesh.textureCoordinates.size() > 0)
    {
        std::vector<glm::vec3> tangents, bitangents;
        computeTangentBasis(mesh.vertices, mesh.textureCoordinates, tangents, bitangents);
        generateAttribute(2, 3, tangents, true);
        generateAttribute(3, 3, bitangents, true);
        generateAttribute(4, 2, mesh.textureCoordinates, false);
    }

    unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

    return vaoID;
}
