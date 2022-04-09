#ifndef MESH_HPP
#define MESH_HPP
#pragma once

#include <fstream>
#include <vector>

#include <cgltf.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "options.hpp"


enum SurfaceType
{
    FLAT,
    CURVE,
    SPHERE
};


struct Mesh
{
public:
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoordinates;


    Mesh() = default;

    Mesh(std::string const &filename, std::string const &root = "../res/models/")
    {
        std::string file = root + filename;
        cgltf_data *data = readData(file.c_str());
        loadData(data);
        free(data);
        if (OPTIONS::verbose) printf("Loaded mesh:  %s\t Vertices: %i Indices: %i\n", file.c_str(), vertices.size(), indices.size());
    }



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
                // Calculate normal from winding order of points
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



private: // Helper methods to load mesh from a .gltf file
    cgltf_data *readData(const char *file)
    {
        std::ifstream fd(file);
        if (fd.fail())
        {
            fprintf(stderr,
                    "Something went wrong when loading model at \"%s\".\n"
                    "The file may not exist or is currently inaccessible.\n",
                    file);
            return nullptr;
        }

        cgltf_options options;
        memset(&options, 0, sizeof(cgltf_options));
        cgltf_data *data = NULL;

        cgltf_result result = cgltf_parse_file(&options, file, &data);
        if (result != cgltf_result_success)
        {
            printf("Failed to parse glTF file: %s\n", file);
            return nullptr;
        }
        result = cgltf_load_buffers(&options, data, file);
        if (result != cgltf_result_success)
        {
            cgltf_free(data);
            printf("Failed to load Buffers: %s\n", file);
            return nullptr;
        }
        result = cgltf_validate(data);
        if (result != cgltf_result_success)
        {
            cgltf_free(data);
            printf("Failed to validate data: %s\n", file);
            return nullptr;
        }
        return data;
    }

    void addAttribute(cgltf_attribute &attribute)
    {
        // Get the accessor of the attribute and its component count
        cgltf_accessor &accessor = *attribute.data;
        unsigned int vecSize     = 0;
        if (accessor.type == cgltf_type_vec2) vecSize = 2;
        if (accessor.type == cgltf_type_vec3) vecSize = 3;
        if (accessor.type == cgltf_type_vec4) vecSize = 4;

        // Read the floats from the accessor
        std::vector<float> attributeFloats;
        attributeFloats.resize(vecSize * accessor.count);
        for (cgltf_size i = 0; i < accessor.count; ++i)
        {
            cgltf_accessor_read_float(&accessor, i, &attributeFloats[i * vecSize], vecSize);
        }

        // Loop over all the values of the accessor
        // Note that accessor.count is not equal to the number of floats in the accessor
        // It's equal to the number of attribute values (e.g, vec2s, vec3s, vec4s, etc.) in the accessor
        cgltf_attribute_type attributeType = attribute.type;
        unsigned int numAttributeValues    = static_cast<unsigned int>(accessor.count);
        for (unsigned int attributeValueIndex = 0; attributeValueIndex < numAttributeValues; ++attributeValueIndex)
        {
            int indexOfFirstFloatOfCurrAttribValue = attributeValueIndex * vecSize;
            switch (attributeType)
            {
                case cgltf_attribute_type_position:
                    // Store a position vec3
                    vertices.push_back(glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]));
                    break;
                case cgltf_attribute_type_texcoord:
                    // Store a texture coordinates vec2
                    textureCoordinates.push_back(glm::vec2(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                           attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1]));
                    break;
                case cgltf_attribute_type_normal:
                {
                    // Store a normal vec3
                    glm::vec3 normal = glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]);
                    normals.push_back(glm::normalize(normal));
                }
                break;
            }
        }
    }

    void loadData(cgltf_data *data)
    {
        // Loop over the array of nodes of the glTF file
        unsigned int numNodes = static_cast<unsigned int>(data->nodes_count);
        for (int nodeIndex = 0; nodeIndex < numNodes; nodeIndex++)
        {
            // This function only loads static meshes, so the node must contain a mesh for us to process it
            cgltf_node *model = &data->nodes[nodeIndex];
            if (model->mesh == nullptr) return;

            // Loop over the array of mesh primitives of the node
            unsigned int numPrimitives = static_cast<unsigned int>(model->mesh->primitives_count);
            for (unsigned int primitiveIndex = 0; primitiveIndex < numPrimitives; ++primitiveIndex)
            {
                // Get the current mesh primitive
                cgltf_primitive *primitive = &model->mesh->primitives[primitiveIndex];

                // If the current mesh primitive has a set of indices, store them
                if (primitive->indices != nullptr)
                {
                    unsigned int indexCount = static_cast<unsigned int>(primitive->indices->count);
                    for (unsigned int i = 0; i < indexCount; ++i)
                    {
                        unsigned int index = static_cast<unsigned int>(cgltf_accessor_read_index(primitive->indices, i));
                        indices.push_back(index);
                    }
                }
                // Loop over the attributes of the current mesh primitive
                unsigned int numAttributes = static_cast<unsigned int>(primitive->attributes_count);
                for (unsigned int attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
                {
                    // Get the current attribute
                    cgltf_attribute *attribute = &primitive->attributes[attributeIndex];
                    // Read the values of the current attribute and store them in the current mesh
                    addAttribute(*attribute);
                }
            }
        }
    }

    void free(cgltf_data *data)
    {
        if (data) cgltf_free(data);
    }
};

#endif
