#pragma once

#include <string>

#include "structs/image_struct.hpp"


namespace IMAGE
{
    Image loadFrom(std::string filename);
    void write(std::string filename, Image image, std::string root = "../images/");
    void write(std::string filename, int width, int height, void *pixels, std::string root = "../images/");
}
