#ifndef MESH_HPP
#define MESH_HPP
#pragma once

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>



enum SurfaceType
{
    FLAT,
    CURVE,
    SPHERE
};


struct Mesh
{
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoordinates;



    void addVertex(glm::vec3 vertex)
    {
        vertices.push_back(vertex);
        indices.push_back(indices.size());
    }



    /** Appends a triangle to the current mesh with the given points, points must be
     * given in counter-clockwise-rotation based on viewing angle. Adds normal and indices.
     * SurfaceType affects how the normals are calculated, for SPHERE and CURVE the normals
     * are assumed to be directly outword from 0.0 to the vertex
     *
     *        p1
     *       /  \
     *      /    \
     *     p2----p3
     */
    void addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, SurfaceType surface = FLAT)
    {
        addVertex(p1);
        addVertex(p3);
        addVertex(p2);

        switch (surface)
        {
            case FLAT:
                normals.push_back(glm::triangleNormal(p1, p3, p2));
                normals.push_back(glm::triangleNormal(p1, p3, p2));
                normals.push_back(glm::triangleNormal(p1, p3, p2));
                break;
            case CURVE:
                normals.push_back(glm::normalize(glm::vec3(p1.x, 0, p1.z)));
                normals.push_back(glm::normalize(glm::vec3(p3.x, 0, p3.z)));
                normals.push_back(glm::normalize(glm::vec3(p2.x, 0, p2.z)));
                break;
            case SPHERE:
                normals.push_back(glm::normalize(p1));
                normals.push_back(glm::normalize(p3));
                normals.push_back(glm::normalize(p2));
                break;
        }
    }

    /** Appends a quad (square) based on four points with correct normals and indices
     * The points must be placed so that when looking at the quad the points are in
     * counter-clockwise-direction
     *
     *     p1----p4
     *     |      |
     *     |      |
     *     p2----p3
     */
    void addQuad(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, SurfaceType surface = FLAT)
    {
        addTriangle(p1, p2, p3, surface);
        addTriangle(p3, p4, p1, surface);
    }
};

#endif