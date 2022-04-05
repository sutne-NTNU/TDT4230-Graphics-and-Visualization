#pragma once

#include <string>

#include "structs/mesh_struct.hpp"


namespace GLTF
{
    Mesh loadMeshFrom(std::string const &filename, std::string const &root = "../res/models/");
}
