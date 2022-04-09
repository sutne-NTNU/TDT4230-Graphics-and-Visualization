#include "program.hpp"

#include <glad/glad.h>

#include "scene.hpp"

std::chrono::steady_clock::time_point previousFrameTime = std::chrono::steady_clock::now();



void runProgram(GLFWwindow *window)
{
    // Configure miscellaneous OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set default colour after clearing the colour buffer
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
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
        updateState(getSecondsSince(&previousFrameTime));
        // Perform a full render pass on the scene
        renderFrame();

        // Handle events (keyboard/mouse input, window resizing etc.)
        glfwPollEvents();

        // Flip buffers
        glfwSwapBuffers(window);
    }
    if (OPTIONS::verbose) printf("Window Was Closed\n");
}
