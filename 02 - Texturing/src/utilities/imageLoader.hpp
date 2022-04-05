#pragma once

#include "lodepng.h"
#include <string>
#include <vector>

typedef struct PNGImage
{
    unsigned int width;
    unsigned int height;
    std::vector<unsigned char> pixels;
} PNGImage;

PNGImage loadPNGFile(std::string fileName);
