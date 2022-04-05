#ifndef OPTIONS_HPP
#define OPTIONS_HPP
#pragma once



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