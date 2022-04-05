#ifndef WINDOW_HPP
#define WINDOW_HPP
#pragma once

#include <string>

#include <glad/glad.h>

namespace WINDOW
{
    // Constants
    const int width         = 1920;
    const int height        = 1080;
    const std::string title = "TDT4230 - Project";
    const GLint resizable   = GL_FALSE;
    const int samples       = 4;
}

#endif
