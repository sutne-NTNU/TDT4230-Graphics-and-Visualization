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
    FrameBuffer refractionFramebuffer;
    std::vector<FrameBuffer> cubemapFramebuffers;

    FrameBufferHandler()
    {
        refractionFramebuffer = FRAMEBUFFER::initFrameBuffer();
        for (unsigned int i = 1; i <= 6; i++)
        {
            cubemapFramebuffers.push_back(FRAMEBUFFER::initFrameBuffer());
        }
        // GLenum DrawBuffers[1] = {
        //     GL_COLOR_ATTACHMENT0
        // };
        // glDrawBuffers(1, DrawBuffers);
    }

    // Draws the refraction framebuffer to the screen
    void activateRefractionBuffer()
    {
        FRAMEBUFFER::bindFramebuffer(refractionFramebuffer.ID);
    }

    // Draws to the cubemap framebuffer
    void activateCubemapBuffer(int index)
    {
        FRAMEBUFFER::bindFramebuffer(cubemapFramebuffers[index].ID);
    }


    // Return to use the default (screen) framebuffer
    void activateScreenBuffer()
    {
        FRAMEBUFFER::unbindFramebuffer();
    }
};

#endif