#include "framebuffer.hpp"

#include <iostream>
#include <string>
#include <vector>


#include <glad/glad.h>
#include <glm/glm.hpp>

#include "cubemap.hpp"
#include "window.hpp"


namespace FRAMEBUFFER
{
    // Verify that the state of the framebuffer is correct, prints error if it isnt.
    void checkFramebufferStatus(std::string errorMessage)
    {
        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status == GL_FRAMEBUFFER_COMPLETE) return;

        fprintf(stderr, "\nError: %s\t%s", errorMessage.c_str(), status);
        assert(("Framebuffer is not okay", false));
    }



    // Creates a new framebuffer, but unbinds it when done (revert back to regular screen buffer)
    Framebuffer initFramebuffer(unsigned int width, unsigned int height)
    {
        // Create the framebuffer
        unsigned int framebufferID;
        glGenFramebuffers(1, &framebufferID);

        // Create the texture the framebuffer will render to
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Create the renderbuffer for depth
        unsigned int renderID;
        glGenRenderbuffers(1, &renderID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        // Bind parts togther
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, framebufferID);

        // Check that the framebuffer is okay
        checkFramebufferStatus("Creating Framebuffer failed");
        // Unbind the framebuffer, we have created it, but we probably arent going to use it just yet
        unbindFramebuffer();
        return Framebuffer { framebufferID, textureID, renderID, width, height };
    }



    Framebuffer initCubemapFramebuffer(unsigned int size)
    {
        // Create the framebuffer
        unsigned int framebufferID;
        glGenFramebuffers(1, &framebufferID);

        // Create the cubemap texture the framebuffer will render to
        unsigned int textureID;
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
        unsigned int renderID;
        glGenRenderbuffers(1, &renderID);
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);

        // Bind parts togther
        glBindFramebuffer(GL_FRAMEBUFFER, framebufferID); // activate this framebuffer, and attch texture and depth buffer to it
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, textureID, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferID);

        checkFramebufferStatus("Creating Cubemap Framebuffer Failed");
        unbindFramebuffer();
        return Framebuffer { framebufferID, textureID, renderID, size, size };
    }



    // Set which framebuffer to render to, clears the buffers
    void bindFramebuffer(Framebuffer framebuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.ID);       // activate this framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // clear the buffers
        glViewport(0, 0, framebuffer.width, framebuffer.height); // update viewport
    }

    // Revert to default (screen) framebuffer
    void unbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);               // unbind framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the buffers
        glViewport(0, 0, WINDOW::width, WINDOW::height);
    }
}