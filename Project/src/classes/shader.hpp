#ifndef SHADER_HPP
#define SHADER_HPP
#pragma once

#include <cassert>
#include <fstream>
#include <memory>
#include <string>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>


// The location of all uniforms not fetched by name
namespace UNIFORMS
{
    const int M = 1;
    const int V = 2;
    const int P = 3;
    const int N = 4;
}

// Texture bindings in fragment shader
namespace BINDINGS
{
    const int skybox        = 0;
    const int texture_map   = 1;
    const int normal_map    = 2;
    const int roughness_map = 3;

    const int reflection_cubemap = 10;
}



class Shader
{
private:
    GLuint mProgram;
    GLint mStatus;
    GLint mLength;

public:
    /**
     * @brief Construct a new Shader object with the vertex and fragment shader filenames
     *
     * @param vertexFilename
     * @param fragmentFilename
     * @param root path to folder that contains the shaders
     */
    Shader(std::string const &vertexFilename,
           std::string const &fragmentFilename,
           std::string const &root = "../shaders/")
    {
        mProgram = glCreateProgram();

        attach(root + vertexFilename);
        attach(root + fragmentFilename);
        link();
    }

    void activate()
    {
        glUseProgram(mProgram);
    }

    void deactivate()
    {
        glUseProgram(0);
    }

    void destroy()
    {
        glDeleteProgram(mProgram);
    }

    GLint getProgram()
    {
        return mProgram;
    }


    /**
     * @brief Get a uniforms ID from a string containing its name
     *
     * @param uniformName name of the uniform
     * @return GLint - the location of the uniform
     */
    GLint getUniformLocation(std::string const &uniformName)
    {
        GLint location = glGetUniformLocation(mProgram, uniformName.c_str());
        return location; // Location is -1 if name is invalid OR the uniform isnt used
    }

    /*
     * Some more convenience funstions so i dont have to think about what gl function and type each uniform is
     */


    template <typename T>
    void setUniform(std::string const &uniformName, T value)
    {
        GLint location = getUniformLocation(uniformName);
        if (location == -1) return;
        setUniform(location, value);
    }


    void setUniform(unsigned int location, int value) { glUniform1i(location, value); }
    void setUniform(unsigned int location, bool value) { glUniform1i(location, value); }
    void setUniform(unsigned int location, float value) { glUniform1f(location, value); }
    void setUniform(unsigned int location, double value) { glUniform1f(location, value); }
    void setUniform(unsigned int location, unsigned int value) { glUniform1i(location, value); }
    void setUniform(unsigned int location, glm::vec2 value) { glUniform2fv(location, 1, glm::value_ptr(value)); }
    void setUniform(unsigned int location, glm::vec3 value) { glUniform3fv(location, 1, glm::value_ptr(value)); }
    void setUniform(unsigned int location, glm::vec4 value) { glUniform4fv(location, 1, glm::value_ptr(value)); }
    void setUniform(unsigned int location, glm::mat3 value) { glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value)); }
    void setUniform(unsigned int location, glm::mat4 value) { glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)); }



private:
    // Disable copying and assignment
    Shader(Shader const &) = delete;
    Shader &operator=(Shader const &) = delete;

    /* Helper function for creating shaders */
    GLuint create(std::string const &filename)
    {
        // Extract file extension and create the correct shader type
        auto idx = filename.rfind(".");
        auto ext = filename.substr(idx + 1);
        if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
        if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
        if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
        if (ext == "tcs") return glCreateShader(GL_TESS_CONTROL_SHADER);
        if (ext == "tes") return glCreateShader(GL_TESS_EVALUATION_SHADER);
        if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
        return false;
    }


    /* Attach a shader to the current shader program */
    void attach(std::string const &filename)
    {
        // Load GLSL Shader from source
        std::ifstream fd(filename.c_str());
        if (fd.fail())
        {
            fprintf(stderr,
                    "Something went wrong when attaching the Shader file at \"%s\".\n"
                    "The file may not exist or is currently inaccessible.\n",
                    filename.c_str());
            return;
        }
        auto src = std::string(std::istreambuf_iterator<char>(fd),
                               (std::istreambuf_iterator<char>()));

        // Create shader object
        const char *source = src.c_str();
        auto shader        = create(filename);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        // Display errors
        glGetShaderiv(shader, GL_COMPILE_STATUS, &mStatus);
        if (!mStatus)
        {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetShaderInfoLog(shader, mLength, nullptr, buffer.get());
            fprintf(stderr, "%s\n%s", filename.c_str(), buffer.get());
        }

        assert(mStatus);

        // Attach shader and free allocated memory
        glAttachShader(mProgram, shader);
        glDeleteShader(shader);
    }

    /* Links all attached shaders together into a shader program */
    void link()
    {
        // Link all attached shaders
        glLinkProgram(mProgram);

        // Display errors
        glGetProgramiv(mProgram, GL_LINK_STATUS, &mStatus);
        if (!mStatus)
        {
            glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
            std::unique_ptr<char[]> buffer(new char[mLength]);
            glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
            fprintf(stderr, "%s\n", buffer.get());
        }

        assert(mStatus);
    }


    /* Used for debugging shader programs (expensive to run) */
    bool isValid()
    {
        // Validate linked shader program
        glValidateProgram(mProgram);

        // Display errors
        glGetProgramiv(mProgram, GL_VALIDATE_STATUS, &mStatus);
        if (mStatus) return true;

        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &mLength);
        std::unique_ptr<char[]> buffer(new char[mLength]);
        glGetProgramInfoLog(mProgram, mLength, nullptr, buffer.get());
        fprintf(stderr, "%s\n", buffer.get());
        return false;
    }
};


#endif
