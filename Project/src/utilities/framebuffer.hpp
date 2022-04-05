#pragma once



struct FrameBuffer
{
    unsigned int ID;
    unsigned int colorBufferID;
    unsigned int depthBufferID;
};



namespace FRAMEBUFFER
{
    FrameBuffer initFrameBuffer();
    void bindFramebuffer(unsigned int framebuffer);
    void unbindFramebuffer();
}
