#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

namespace STB
{

    unsigned char *load(const char *filename, int *width, int *height, int *comp, int req_comp)
    {
        return stbi_load(filename, width, height, comp, req_comp);
    }

    void write(const char *filename, int width, int height, void *data)
    {
        stbi_flip_vertically_on_write(true);
        stbi_write_png(filename, width, height, 4, data, 0);
    }
}

namespace CGLTF
{

}