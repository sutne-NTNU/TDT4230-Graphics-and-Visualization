#include "framebuffer.hpp"

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "window.hpp"

namespace FRAMEBUFFER
{
    // Initialize a new FrameBufferObject and return its ID
    unsigned int generateBuffer()
    {
        unsigned int frameBufferID;
        glGenFramebuffers(1, &frameBufferID);
        return frameBufferID;
    }

    /**
     * Attach a color texture to a framebuffer
     */
    unsigned int createColorBufferTextureAttachment()
    {
        unsigned int colorBufferID;
        glGenTextures(1, &colorBufferID);
        glBindTexture(GL_TEXTURE_2D, colorBufferID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW::width, WINDOW::height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        return colorBufferID;
    }

    unsigned int createDepthBuffer()
    {
        unsigned int depthBufferID;
        glGenRenderbuffers(1, &depthBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW::width, WINDOW::height);
        return depthBufferID;
    }



    // Creates a new framebuffer, but unbinds it when done (revert back to regular screen buffer)
    FrameBuffer initFrameBuffer()
    {
        unsigned int id          = generateBuffer();
        unsigned int colorBuffer = createColorBufferTextureAttachment();
        unsigned int depthBuffer = createDepthBuffer();

        // Bind parts togther
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR: Framebuffer is not complete" << std::endl;

        unbindFramebuffer();
        return FrameBuffer { id, colorBuffer, depthBuffer };
    }

    // Set which framebuffer to render to, clears the buffers
    void bindFramebuffer(unsigned int framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, WINDOW::width, WINDOW::height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Revert to default (screen) framebuffer
    void unbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW::width, WINDOW::height);
    }
}