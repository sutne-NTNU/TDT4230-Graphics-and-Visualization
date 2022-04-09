#ifndef PROGRAM_HPP
#define PROGRAM_HPP
#pragma once

#include <chrono>
#include <thread>

#include <GLFW/glfw3.h>

#include "options.hpp"



void runProgram(GLFWwindow *window);



/** Return nanoseconds since the given time point */
inline long nanoSince(std::chrono::steady_clock::time_point time)
{
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - time).count();
}



/** Return number of seconds since the last time this function was called */
inline float getSecondsSince(std::chrono::steady_clock::time_point *previousTime)
{
    long frameDelta = nanoSince(*previousTime);
    if (OPTIONS::limitFPS)
    {
        long targetFrameDelta = 1000000000 / (OPTIONS::fpsLimit * 1.3); // in nanoseconds
        if (frameDelta < targetFrameDelta)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(targetFrameDelta - frameDelta));
            frameDelta = nanoSince(*previousTime);
        }
    }
    // Convert and return time in seconds
    float frameSeconds = frameDelta / 1000000000.0;
    *previousTime      = std::chrono::steady_clock::now();
    return frameSeconds;
}

#endif