#include "image.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "options.hpp"


namespace IMAGE
{


    /**
     * @brief Takes a filename and loads the data to an Image struct
     *
     * @param filename The filename of the image to load
     * @return Image
     */
    Image loadFrom(std::string filename)
    {
        // Load image
        int width, height, channels;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
        if (!data)
        {
            std::cout << "Failed to load image: " << filename << std::endl;
            return Image {};
        }
        // Create and return image struct
        if (channels < 1 || 4 < channels)
        {
            std::cout << "loadImageFrom(): Invalid number of channels: " << channels << std::endl;
            return Image {};
        }

        Image image;
        image.width  = width;
        image.height = height;
        for (int i = 0; i < width * height * 4; i += 4)
        {
            Pixel pixel;
            pixel.r = data[i];
            if (channels >= 2) pixel.g = data[i + 1];
            if (channels >= 3) pixel.b = data[i + 2];
            if (channels >= 4) pixel.a = data[i + 3];
            image.pixels.push_back(pixel);
        }
        if (OPTIONS::verbose) std::cout << "Loaded image: " << filename << " \tWidth: " << width << " Height: " << height << " Channels: " << channels << std::endl;
        return image;
    }



    void write(std::string filename, Image image, std::string root)
    {
        write(filename, image.width, image.height, image.pixels.data(), root);
    }

    /**
     * @brief Write an image to a file
     *
     * @param filename The name of the saved file
     * @param width The width of the image (in pixels)
     * @param height The height of the image (in pixels)
     * @param pixels The pixels of the image
     * @param root Direcotry of the saved file
     */
    void write(std::string filename, int width, int height, void *pixels, std::string root)
    {
        // Files are saved relative to build/
        std::string file = root + filename;
        stbi_flip_vertically_on_write(true);
        stbi_write_png(file.c_str(), width, height, 4, pixels, 0);
        std::cout << "Wrote image: " << file << std::endl;
        free(pixels);
    }
}