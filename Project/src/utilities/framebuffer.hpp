#pragma once

#include <string>

struct Framebuffer
{
    unsigned int ID;
    unsigned int textureID;
    unsigned int renderID;
    unsigned int width;
    unsigned int height;
};



namespace FRAMEBUFFER
{
    Framebuffer initFramebuffer(unsigned int width, unsigned int height);
    Framebuffer initCubemapFramebuffer(unsigned int size);
    void bindFramebuffer(Framebuffer framebuffer);
    void unbindFramebuffer();
    void checkFramebufferStatus(std::string errorMessage);
}
