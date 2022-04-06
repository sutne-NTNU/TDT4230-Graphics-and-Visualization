#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "options.hpp"
#include "utilities/cubemap.hpp"
#include "utilities/framebuffer.hpp"
#include "window.hpp"



class FrameBufferHandler
{
public:
    Framebuffer refractionFramebuffer;
    Framebuffer cubemapFrameBuffer;

    FrameBufferHandler()
    {
        refractionFramebuffer = FRAMEBUFFER::initFramebuffer(WINDOW::width, WINDOW::height);
        cubemapFrameBuffer    = FRAMEBUFFER::initCubemapFramebuffer(WINDOW::width);
    }

    // Activates the refraction framebuffer, rendering will be done to its texture
    void activateRefractionBuffer()
    {
        FRAMEBUFFER::bindFramebuffer(refractionFramebuffer);
        glActiveTexture(GL_TEXTURE0);
    }

    // Activate the cubemap framebuffer, must also call selectCubemapTarget() to render to correct side.
    void activateCubemapBuffer()
    {
        FRAMEBUFFER::bindFramebuffer(cubemapFrameBuffer);
    }

    // draw to the given cubemap textures side (0 = right, 1 = left, 2 = top, 3 = bottom, 4 = front, 5 = back)
    // Must be called after activateCubemapBuffer()
    void selectCubemapTarget(unsigned int side)
    {
        // we have to update to the correct cubemap texture, otherwise they would all render to the same side (right)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CUBEMAP::sides[side], cubemapFrameBuffer.textureID, 0);
        // Clear this side of the cubemap before rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }


    // Return to use the default (screen) framebuffer
    void activateScreenBuffer()
    {
        FRAMEBUFFER::unbindFramebuffer();
    }
};

#endif