#ifndef OPTIONS_HPP
#define OPTIONS_HPP
#pragma once

#include <string>

namespace WINDOW
{
    const std::string title = "TDT4230 - Project";
    const int width         = 1920;
    const int height        = 1080;
    const bool resizable    = false;
    const int samples       = 4;
}



namespace OPTIONS
{
    enum MODE
    {
        DEBUG, // load less resources (fewer skyboxes)
        TEST,  // load everything but still low resolution textures
        DEMO   // load everything with highest-res textures
    };

    const MODE mode    = DEBUG;
    const bool verbose = true;
}

#endif