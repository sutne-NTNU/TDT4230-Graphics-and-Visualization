#ifndef WRAPPERS_WRAPPER_HPP
#define WRAPPERS_WRAPPER_HPP
#pragma once

namespace STB
{
    unsigned char *load(const char *filename, int *width, int *height, int *comp, int req_comp);
    void write(const char *filename, int width, int height, void *data);
}

namespace CGLTF
{

}

#endif