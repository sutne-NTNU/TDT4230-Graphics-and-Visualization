#pragma once

#include <string>



namespace CUBEMAP
{
    unsigned int generateBuffer();
    unsigned int load(std::string name, std::string extension, std::string root = "../res/cubemaps/");
}
