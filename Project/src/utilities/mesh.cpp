#include "mesh.hpp"

#include <fstream>

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "options.hpp"
#include "structs/mesh_struct.hpp"



namespace GLTF
{
    cgltf_data *readData(std::string const &file)
    {
        std::ifstream fd(file.c_str());
        if (fd.fail())
        {
            fprintf(stderr,
                    "Something went wrong when loading model at \"%s\".\n"
                    "The file may not exist or is currently inaccessible.\n",
                    file.c_str());
            return nullptr;
        }

        cgltf_options options;
        memset(&options, 0, sizeof(cgltf_options));
        cgltf_data *data = NULL;

        cgltf_result result = cgltf_parse_file(&options, file.c_str(), &data);
        if (result != cgltf_result_success)
        {
            printf("Failed to parse glTF file: %s\n", file);
            return nullptr;
        }
        result = cgltf_load_buffers(&options, data, file.c_str());
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



    void free(cgltf_data *data)
    {
        if (data) cgltf_free(data);
    }



    void addAttributeToMesh(cgltf_attribute &attribute, Mesh *mesh)
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
                    mesh->vertices.push_back(glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                       attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                                       attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]));
                    break;
                case cgltf_attribute_type_texcoord:
                    // Store a texture coordinates vec2
                    mesh->textureCoordinates.push_back(glm::vec2(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1]));
                    break;
                case cgltf_attribute_type_normal:
                {
                    // Store a normal vec3
                    glm::vec3 normal = glm::vec3(attributeFloats[indexOfFirstFloatOfCurrAttribValue + 0],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 1],
                                                 attributeFloats[indexOfFirstFloatOfCurrAttribValue + 2]);
                    mesh->normals.push_back(glm::normalize(normal));
                }
                break;
            }
        }
    }



    void loadDataToMesh(cgltf_data *data, Mesh *mesh)
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
                        mesh->indices.push_back(index);
                    }
                }
                // Loop over the attributes of the current mesh primitive
                unsigned int numAttributes = static_cast<unsigned int>(primitive->attributes_count);
                for (unsigned int attributeIndex = 0; attributeIndex < numAttributes; ++attributeIndex)
                {
                    // Get the current attribute
                    cgltf_attribute *attribute = &primitive->attributes[attributeIndex];
                    // Read the values of the current attribute and store them in the current mesh
                    addAttributeToMesh(*attribute, mesh);
                }
            }
        }
    }



    Mesh loadMeshFrom(std::string const &filename, std::string const &root)
    {
        Mesh mesh;
        std::string file = root + filename;
        cgltf_data *data = readData(file);
        loadDataToMesh(data, &mesh);
        free(data);
        if (OPTIONS::verbose) printf("Loaded mesh:  %s\t Vertices: %i Indices: %i\n", file.c_str(), mesh.vertices.size(), mesh.indices.size());
        return mesh;
    }
}
