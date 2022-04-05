#pragma once

#include "structs/mesh_struct.hpp"

namespace SHAPES
{
    // Affects detail level of round shapes
    const float detailLevel = 10.0;

    Mesh Cube(float size);
    Mesh Pyramid(float height, float baseWidth);
    Mesh Prism(float height, float width, float thickness);
    Mesh Cylinder(float radius, float height);
    Mesh Sphere(float radius);
}