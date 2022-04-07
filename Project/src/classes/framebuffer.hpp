#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "options.hpp"
#include "utilities/cubemap.hpp"
#include "window.hpp"


/**
 * Specialized frambuffer class, that only handles cubemap framebuffers (as that is the only use case for this project)
 */
class Framebuffer
{
public:
    unsigned int ID;
    unsigned int textureID;
    unsigned int depthID;

    unsigned int size;

    /**
     * @param size height and width of the cube, affects the resoultion of the cubemap
     */
    Framebuffer(unsigned int size)
    {
        // Create the framebuffer
        glGenFramebuffers(1, &ID);

        // Create the cubemap texture the framebuffer will render to
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        for (GLenum side : CUBEMAP::sides)
        {
            glTexImage2D(side, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Create the depth render buffer
        glGenRenderbuffers(1, &depthID);
        glBindRenderbuffer(GL_RENDERBUFFER, depthID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);

        // Bind parts togther
        glBindFramebuffer(GL_FRAMEBUFFER, ID); // activate this framebuffer, and attach texture and depth buffer to it
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureID, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ID);

        checkFramebufferStatus("Creating Cubemap Framebuffer Failed");
        Framebuffer::activateScreen(); // Revert to screen framebuffer after creation
        this->size = size;
    }

    // Render to this framebuffer
    void activate()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ID);              // activate this framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the buffers
        glViewport(0, 0, size, size);                       // update viewport
    }

    // draw to the given cubemap textures side:
    //
    //      0 = GL_TEXTURE_CUBE_MAP_POSITIVE_X / right
    //      1 = GL_TEXTURE_CUBE_MAP_NEGATIVE_X / left
    //      2 = GL_TEXTURE_CUBE_MAP_POSITIVE_Y / top
    //      3 = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y / bottom
    //      4 = GL_TEXTURE_CUBE_MAP_POSITIVE_Z / front
    //      5 = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z / back
    //
    // Must be called after this->activate()
    void selectRenderTargetSide(unsigned int side)
    {
        // we have to update to the correct cubemap texture, otherwise they would all render to the same side (right)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, CUBEMAP::sides[side], textureID, 0);
        // Clear this side of the cubemap before rendering
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // Render to default (screen) framebuffer
    static void activateScreen()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);               // unbind framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the buffers
        glViewport(0, 0, WINDOW::width, WINDOW::height);
    }


private:
    // Verify that the state of the framebuffer is correct, prints error if it isnt.
    void checkFramebufferStatus(std::string errorMessage)
    {
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status == GL_FRAMEBUFFER_COMPLETE) return;

        fprintf(stderr, "\nError: %s\t%s", errorMessage.c_str(), status);
        assert(("Framebuffer is not okay", false));
    }
};

#endif