#ifndef CAMERA_HPP
#define CAMERA_HPP
#pragma once

#include <glm/glm.hpp>

#include "utilities/utils.hpp"

class Camera
{
private:
    // Camera settings
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;

    // Last cursor position
    GLfloat CursorXPos = 0.0f;
    GLfloat CursorYPos = 0.0f;

public:
    // View Vectors
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    // Camera Attributes
    glm::vec3 position;
    // Rotations
    GLfloat pitch;
    GLfloat yaw;

    Camera(
        glm::vec3 startPosition = glm::vec3(0, 0, 0),
        float startPitch        = 0.0f,
        float startYaw          = -90.0f)
    {
        position = startPosition;
        pitch    = startPitch;
        yaw      = startYaw;
        updateCameraViewVectors();
        resetSensitivity();
    }


    /** Handle cursor position from a callback mechanism */
    void handleCursorPosition(float newX, float newY)
    {
        GLfloat deltaX = newX - CursorXPos;
        GLfloat deltaY = CursorYPos - newY;
        CursorXPos     = newX;
        CursorYPos     = newY;

        pitch += MouseSensitivity * deltaY;
        yaw += MouseSensitivity * deltaX;

        // Prevent flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraViewVectors();
    }

    void updatePosition(GLfloat deltaTime, bool keyIsPressed[512])
    {
        GLfloat velocity = MovementSpeed * deltaTime;
        if (keyIsPressed[GLFW_KEY_W]) position += front * velocity;    // forward
        if (keyIsPressed[GLFW_KEY_S]) position -= front * velocity;    // backward
        if (keyIsPressed[GLFW_KEY_D]) position += right * velocity;    // right
        if (keyIsPressed[GLFW_KEY_A]) position -= right * velocity;    // left
        if (keyIsPressed[GLFW_KEY_SPACE]) position.y += velocity;      // vertical up
        if (keyIsPressed[GLFW_KEY_LEFT_SHIFT]) position.y -= velocity; // vertical down
    }

    void increaseSensitivity()
    {
        MouseSensitivity += MouseSensitivity / 2;
        MovementSpeed += MovementSpeed;
    }

    void decreaseSensitivity()
    {
        MouseSensitivity -= MouseSensitivity / 4;
        MovementSpeed -= MovementSpeed / 2;
    }

    void resetSensitivity()
    {
        MouseSensitivity = 0.1f;
        MovementSpeed    = 30;
    }

private:
    void updateCameraViewVectors()
    {
        glm::vec3 newFront = glm::vec3(
            cos(glm::radians(yaw)) * cos(glm::radians(pitch)), // X
            sin(glm::radians(pitch)),                          // Y
            sin(glm::radians(yaw)) * cos(glm::radians(pitch))  // Z
        );

        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
        up    = glm::normalize(glm::cross(right, front));
    }
};

#endif