#include "options.hpp"
#include "program.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

// A callback which allows GLFW to report errors whenever they occur
static void glfwErrorCallback(int error, const char *description)
{
    if (error != GL_NO_ERROR)
    {
        std::string errorString;

        switch (error)
        {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                errorString = "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                errorString = "GL_STACK_OVERFLOW";
                break;
            default:
                errorString = "[Unknown error ID]";
                break;
        }
        fprintf(stderr, "An OpenGL error occurred (%i): %s.\n", error, errorString.c_str());
    }
}

GLFWwindow *initialise()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Could not start GLFW\n");
        exit(EXIT_FAILURE);
    }

    // Set core window options (adjust version numbers if needed)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Enable the GLFW runtime error callback function defined previously.
    glfwSetErrorCallback(glfwErrorCallback);

    // Set additional window options
    glfwWindowHint(GLFW_RESIZABLE, WINDOW::resizable);
    glfwWindowHint(GLFW_SAMPLES, WINDOW::samples); // MSAA

    // Create window using GLFW
    GLFWwindow *window = glfwCreateWindow(WINDOW::width, WINDOW::height, WINDOW::title.c_str(), nullptr, nullptr);

    // Ensure the window is set up correctly
    if (!window)
    {
        fprintf(stderr, "Could not open GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Let the window be the current OpenGL context and initialise glad
    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Print various OpenGL information to stdout
    if (OPTIONS::verbose)
    {
        printf("%s: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
        printf("GLFW\t %s\n", glfwGetVersionString());
        printf("OpenGL\t %s\n", glGetString(GL_VERSION));
        printf("GLSL\t %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    return window;
}



int main(int argc, const char *argb[])
{
    // Initialise window using GLFW
    GLFWwindow *window = initialise();

    // Run an OpenGL application using this window
    runProgram(window);

    // Terminate GLFW (no need to call glfwDestroyWindow)
    glfwTerminate();

    return EXIT_SUCCESS;
}
