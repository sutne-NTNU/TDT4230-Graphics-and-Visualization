#include "program.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include <glad/glad.h>

#include "options.hpp"
#include "scene.hpp"



/** Return nanoseconds since the given time point */
long nanoSince(std::chrono::steady_clock::time_point time)
{
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - time).count();
}

static std::chrono::steady_clock::time_point _previousFrameTime = std::chrono::steady_clock::now();
/** Return number of seconds since the last time this function was called */
float getSecondsSinceLastFrame()
{
    long frameDelta = nanoSince(_previousFrameTime);
    if (OPTIONS::limitFPS)
    {
        long targetFrameDelta = 1000000000 / (OPTIONS::fpsLimit * 1.3); // in nanoseconds
        if (frameDelta < targetFrameDelta)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(targetFrameDelta - frameDelta));
            frameDelta = nanoSince(_previousFrameTime);
        }
    }
    _previousFrameTime = std::chrono::steady_clock::now();

    // Convert the time delta in nanoseconds to seconds
    float frameSeconds = frameDelta / 1000000000.0;

    // std::cout << "FPS: " << (int)(1.0 / frameSeconds) << std::endl;

    return frameSeconds;
}


void runProgram(GLFWwindow *window)
{
    // Configure miscellaneous OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set default colour after clearing the colour buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Place cursor in 0, 0  so deltaX and deltaY are 0 first frame
    glfwSetCursorPos(window, 0, 0);

    if (OPTIONS::verbose) printf("Initializing Scene\n");
    initScene(window);

    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // keep cursor hidden and active
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Update state of entire scene (positions, rotations, cameramovement etc.)
        updateState(getSecondsSinceLastFrame());
        // Perform a full render pass on the scene
        renderFrame();

        // Handle events (keyboard/mouse input, window resizing etc.)
        glfwPollEvents();

        // Flip buffers
        glfwSwapBuffers(window);
    }
    if (OPTIONS::verbose) printf("Window Was Closed\n");
}
