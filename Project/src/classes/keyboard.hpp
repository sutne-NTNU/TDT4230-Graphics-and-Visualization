#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP
#pragma once

#include <GLFW/glfw3.h>

class Keyboard
{
public:
    bool keysCurrentlyPressed[512] = { false };

    Keyboard() { }

    void handleKeyAction(int key, int action)
    {
        if (key < 0 || 512 <= key) return;
        if (action == GLFW_PRESS) keysCurrentlyPressed[key] = true;
        if (action == GLFW_RELEASE) keysCurrentlyPressed[key] = false;
    }
};

#endif