#ifndef IMAGE_HPP
#define IMAGE_HPP
#pragma once

#include <fstream>
#include <iostream>
#include <vector>

#include "options.hpp"
#include "utilities/wrappers.hpp"



struct Pixel
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 1;
};



// Wrapper for stb_image, contains the image data as RGBA and the width and height of the image
class Image
{
public:
    std::vector<Pixel> pixels;
    int width;  // number of pixels per row
    int height; // number of pixels per column

    Image() = default;

    /**
     * @brief Loads image form the file and stores it to easy to read variables
     *
     * @param filename The filename of the image to load
     */
    Image(const std::string &filename)
    {
        // Load image
        int channels;
        unsigned char *data = STB::load(filename.c_str(), &width, &height, &channels, 4);
        if (!data)
        {
            std::cout << "Failed to load image: " << filename << std::endl;
            return;
        }
        // Create and return image struct
        if (channels < 1 || 4 < channels)
        {
            std::cout << "Image " << filename << ": Invalid number of channels: " << channels << std::endl;
            return;
        }
        for (int i = 0; i < width * height * 4; i += 4)
        {
            Pixel pixel;
            pixel.r = data[i];
            if (channels >= 2) pixel.g = data[i + 1];
            if (channels >= 3) pixel.b = data[i + 2];
            if (channels >= 4) pixel.a = data[i + 3];
            pixels.push_back(pixel);
        }
        if (OPTIONS::verbose) std::cout << "Loaded image: " << filename << " \tWidth: " << width << " Height: " << height << " Channels: " << channels << std::endl;
    }


    /**
     * @brief Save this image to a file
     *
     * @param filename The filename of the image to save
     */
    void write(std::string filename, std::string root = "../images/")
    {
        Image::write(filename, width, height, pixels.data(), root);
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
    static void write(std::string filename, int width, int height, void *pixels, std::string root = "../images/")
    {
        std::string file = root + filename;
        STB::write(file.c_str(), width, height, pixels);
        std::cout << "Wrote image: " << file << std::endl;
        free(pixels);
    }



    // Flips the image vertically
    //
    //  |----|      |----|
    //  | x  |      |    |
    //  |    |  ->  | x  |
    //  |----|      |----|
    void flipY()
    {
        for (unsigned int x = 0; x < width; x++)
        {
            for (unsigned int y = 0; y < (height / 2); y++)
            {
                Pixel *topPixel = getPixel(x, height - y - 1);
                Pixel *botPixel = getPixel(x, y);
                swap(topPixel, botPixel);
            }
        }
    }

    // Flips the image horizontally
    //
    //  |------|      |------|
    //  | x    |  ->  |    x |
    //  |------|      |------|
    void flipX()
    {
        for (unsigned int x = 0; x < width / 2; x++)
        {
            for (unsigned int y = 0; y < height; y++)
            {
                Pixel *leftPixel  = getPixel(x, y);
                Pixel *rightPixel = getPixel(width - 1 - x, y);
                swap(leftPixel, rightPixel);
            }
        }
    }

    // Rotates the image 90 degrees clockwise
    //
    //                |----|
    //  |------|      | x  |
    //  | x    |  ->  |    |
    //  |------|      |----|
    void rotate90ClockWise()
    {
        unsigned int newWidth = height, newHeight = width;

        std::vector<Pixel> newPixels(newWidth * newHeight);
        for (unsigned int newX = 0; newX < newWidth; newX++)
        {
            for (unsigned int newY = 0; newY < newHeight; newY++)
            {
                unsigned int newIndex = newY * newWidth + newX;
                Pixel *oldPixel       = getPixel(newY, newX);
                newPixels[newIndex]   = *oldPixel;
            }
        }
        pixels = newPixels;
        width  = newWidth;
        height = newHeight;
    }


private:
    // get pointer to pixel at (x, y)
    Pixel *getPixel(int x, int y)
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            std::cout << "Image::getPixel: Pixel out of bounds" << std::endl;
            return nullptr;
        }
        return &pixels[y * width + x];
    }

    // Swap places for the two pixels
    void swap(Pixel *p1, Pixel *p2)
    {
        Pixel temp = *p1;
        *p1        = *p2;
        *p2        = temp;
    }
};

#endif