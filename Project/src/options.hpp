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
        DEBUG, // load less resources (only one skybox, low res textures)
        TEST,  // load all skyboxes but still low resolution textures
        DEMO,  // load everything with highest-res textures
    };

    const MODE mode    = DEBUG;
    const bool verbose = true; // false = only print errors

    const bool limitFPS = true;
    const int fpsLimit  = 60;

    const float cameraFOV         = 60;
    const float nearClippingPlane = 0.01f;
    const float farClippingPlane  = 300.0f;

    const int environmentBufferResolution = 2048; // Can also be adjusted with arrow keys
}

#endif