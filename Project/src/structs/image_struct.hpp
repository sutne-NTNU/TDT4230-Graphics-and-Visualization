#ifndef IMAGE_HPP
#define IMAGE_HPP
#pragma once

#include <iostream>
#include <vector>

struct Pixel
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 1;
};


// Store image information in a uniform way, containing width, height, and pixels (RGBA)
struct Image
{
    int width;  // number of pixels per row
    int height; // number of pixels per column

    std::vector<Pixel> pixels;



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
};

#endif