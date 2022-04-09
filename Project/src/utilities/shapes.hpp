#pragma once

#include "classes/mesh.hpp"

namespace SHAPES
{
    Mesh Cube(float size);
    Mesh Pyramid(float height, float baseWidth);
    Mesh Prism(float height, float width, float thickness);
    Mesh Cylinder(float radius, float height);
    Mesh Sphere(float radius);
}