#include <GLFW/glfw3.h>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <chrono>
#include <fmt/format.h>
#include <glad/glad.h>
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>

#include "gamelogic.h"
#include "sceneGraph.hpp"
#include <timestamps.h>
#include <utilities/glutils.h>
#include <utilities/mesh.h>
#include <utilities/shader.hpp>
#include <utilities/shapes.h>
#include <utilities/timeutils.h>

CommandLineOptions options;
sf::SoundBuffer *buffer;
Gloom::Shader *shader;
sf::Sound *sound;

double mouseSensitivity = 1.0;
double lastMouseX       = windowWidth / 2;
double lastMouseY       = windowHeight / 2;
bool mouseLeftPressed   = false;
bool mouseLeftReleased  = false;
bool mouseRightPressed  = false;
bool mouseRightReleased = false;

const float debug_startTime = 0;
double totalElapsedTime     = debug_startTime;
double gameElapsedTime      = debug_startTime;

unsigned int currentKeyFrame  = 0;
unsigned int previousKeyFrame = 0;
bool jumpedToNextFrame        = false;

bool hasStarted = false;
bool hasLost    = false;
bool isPaused   = false;



double padPositionX = 0;
double padPositionZ = 0;

const glm::vec3 boxDimensions(180, 90, 90);
const glm::vec3 padDimensions(20, 3, 20);

double ballRadius = 3.0f;
glm::vec3 ballPosition(0, ballRadius + padDimensions.y, boxDimensions.z / 2);
glm::vec3 ballDirection(1, 1, 0.2f);

const glm::vec3 cameraPosition(0, 2, -20);



SceneNode *root;
SceneNode *box;
SceneNode *pad;
SceneNode *ball;

unsigned const int numLights = 4;
std::vector<SceneNode *> lights(numLights);



void mouseCallback(GLFWwindow *window, double x, double y)
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    padPositionX -= mouseSensitivity * deltaX / windowWidth;
    padPositionZ -= mouseSensitivity * deltaY / windowHeight;

    if (padPositionX > 1) padPositionX = 1;
    if (padPositionX < 0) padPositionX = 0;
    if (padPositionZ > 1) padPositionZ = 1;
    if (padPositionZ < 0) padPositionZ = 0;

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}



/**
 * @brief Initializes a SceneNode with VAO from a mesh
 *
 * @param mesh
 * @return SceneNode*
 */
SceneNode *initMesh(Mesh mesh)
{
    unsigned int vao    = generateBuffer(mesh);
    SceneNode *node     = createSceneNode();
    node->vaoID         = vao;
    node->vaoIndexCount = mesh.indices.size();
    return node;
}



/**
 * @brief Executed once before main game loop
 *
 * @param window
 * @param gameOptions
 */
void initGame(GLFWwindow *window, CommandLineOptions gameOptions)
{
    buffer = new sf::SoundBuffer();
    if (!buffer->loadFromFile("../res/Hall of the Mountain King.ogg")) return;

    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

    shader = new Gloom::Shader();
    shader->makeBasicShader("../res/shaders/simple.vert", "../res/shaders/simple.frag");
    shader->activate();

    // Create And Inititalize Nodes and SceneGraph
    root = createSceneNode();
    initObjects();

    getTimeDeltaSeconds();
    std::cout << "Ready. Click to start!" << std::endl;
}



/**
 * @brief Give all objects and camera their dimensions and starting positions/directions
 */
void initObjects()
{
    // Construct Objects
    box  = initMesh(cube(boxDimensions, glm::vec2(90), true, true));
    pad  = initMesh(cube(padDimensions, glm::vec2(30, 40), true));
    ball = initMesh(generateSphere(1.0, 40, 40));

    // Construct Lights
    for (auto &light : lights) light = createLightNode(POINT_LIGHT);

    // Build SceneGraph
    addChild(root, box);
    addChild(root, pad);
    addChild(root, ball);
    addChild(root, lights[0]);
    addChild(root, lights[1]);
    addChild(root, lights[2]);
    addChild(root, lights[3]);

    // Place Objects in their initial positions
    // // x = left/right          positive = right
    // // y = up/down,            positive = up
    // // z = forwards/backwards  positive = backwards (closer to camera)
    box->position       = { 0, -10, -80 };
    lights[0]->position = { 0, 100, 0 }; // will be placed right above ball (create shadow directly under)
    lights[1]->position = { -15, -30, -70 };
    lights[2]->position = { 0, -30, -70 };
    lights[3]->position = { 15, -30, -70 };

    // Attributes
    lights[0]->lightColor = { 1, 1, 1 };
    lights[1]->lightColor = { 1, 0, 0 };
    lights[2]->lightColor = { 0, 1, 0 };
    lights[3]->lightColor = { 0, 0, 1 };

    // Ball has to be scaled, creating it with ballradius makes it appear black
    ball->scale = glm::vec3(ballRadius);
}


/**
 * @brief Executed At start of each new fram, before rendering
 *
 * @param window
 */
void updateFrame(GLFWwindow *window)
{
    updateGameState(window);

    float lookRotation = -0.6 / (1 + exp(-5 * (padPositionX - 0.5))) + 0.3; // Some math to make the camera move in a nice way
    // Calculating ViewProjection Matrix
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);

    glm::mat4 view = glm::rotate(0.3f + 0.2f * float(-padPositionZ * padPositionZ), glm::vec3(1, 0, 0))
                   * glm::rotate(lookRotation, glm::vec3(0, 1, 0))
                   * glm::translate(-cameraPosition);

    glm::mat4 VP = projection * view;

    updateNodeTransformations(root, glm::mat4(1), VP);
}



/**
 * @brief Updates transformations for node recursively for its children
 *
 * @param node
 * @param M Model Transformation Matrix
 * @param VP ViewProjection Transformation Matrix
 */
void updateNodeTransformations(SceneNode *node, glm::mat4 M, glm::mat4 VP)
{
    glm::mat4 myTransformation = glm::translate(node->position)
                               * glm::translate(node->referencePoint)
                               * glm::rotate(node->rotation.y, glm::vec3(0, 1, 0))
                               * glm::rotate(node->rotation.x, glm::vec3(1, 0, 0))
                               * glm::rotate(node->rotation.z, glm::vec3(0, 0, 1))
                               * glm::scale(node->scale)
                               * glm::translate(-node->referencePoint);

    node->M   = M * myTransformation;
    node->MVP = VP * node->M;
    node->N   = glm::mat3(glm::transpose(glm::inverse(node->M)));

    switch (node->type)
    {
        case GEOMETRY:
            break;
        case POINT_LIGHT:
        case SPOT_LIGHT:
            glm::vec4 origin    = glm::vec4(0, 0, 0, 1);
            node->lightPosition = glm::vec3(node->M * origin);
            break;
    }

    for (SceneNode *child : node->children)
    {
        updateNodeTransformations(child, node->M, VP);
    }
}



/**
 * @brief Executed after the frame has been updated
 *
 * @param window
 */
void renderFrame(GLFWwindow *window)
{
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    // Pass Camera Position to Fragment Shader (for specular highlights)
    glUniform3fv(shader->getUniformFromName("camera_position"), 1, glm::value_ptr(cameraPosition));
    // Pass Ball Position and Radius to Fragment shader (for shadows)
    glUniform3fv(shader->getUniformFromName("ball.position"), 1, glm::value_ptr(ball->position));
    glUniform1f(shader->getUniformFromName("ball.radius"), ballRadius);

    renderNode(root);
}



void renderNode(SceneNode *node)
{
    // Pass transformation matrices to vertex shader
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(node->M));
    glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(node->MVP));
    glUniformMatrix3fv(5, 1, GL_FALSE, glm::value_ptr(node->N));

    switch (node->type)
    {
        case GEOMETRY:
            if (node->vaoID != -1)
            {
                glBindVertexArray(node->vaoID);
                glDrawElements(GL_TRIANGLES, node->vaoIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case POINT_LIGHT:
        case SPOT_LIGHT:
            // Pass light position and color to fragment shader
            std::string uniformPosition = fmt::format("lights[{}].position", node->lightID);
            glUniform3fv(shader->getUniformFromName(uniformPosition), 1, glm::value_ptr(node->lightPosition));
            std::string uniformColor = fmt::format("lights[{}].color", node->lightID);
            glUniform3fv(shader->getUniformFromName(uniformColor), 1, glm::value_ptr(node->lightColor));
            break;
    }

    for (SceneNode *child : node->children)
    {
        renderNode(child);
    }
}



/**
 * @brief Updates positions/rotations, checks for colissions, if game is paused etc.
 *
 */
void updateGameState(GLFWwindow *window)
{
    double timeDelta = getTimeDeltaSeconds();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
        mouseLeftPressed  = true;
        mouseLeftReleased = false;
    }
    else
    {
        mouseLeftReleased = mouseLeftPressed;
        mouseLeftPressed  = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    {
        mouseRightPressed  = true;
        mouseRightReleased = false;
    }
    else
    {
        mouseRightReleased = mouseRightPressed;
        mouseRightPressed  = false;
    }

    const float ballBottomY = box->position.y - (boxDimensions.y / 2) + ballRadius + padDimensions.y;
    const float ballTopY    = box->position.y + (boxDimensions.y / 2) - ballRadius;

    const float BallVerticalTravelDistance = ballTopY - ballBottomY;

    // Arbitrary addition to prevent ball from going too much into camera
    const float cameraWallOffset = 30;

    const float ballMinX = box->position.x - (boxDimensions.x / 2) + ballRadius;
    const float ballMaxX = box->position.x + (boxDimensions.x / 2) - ballRadius;
    const float ballMinZ = box->position.z - (boxDimensions.z / 2) + ballRadius;
    const float ballMaxZ = box->position.z + (boxDimensions.z / 2) - ballRadius - cameraWallOffset;

    if (!hasStarted)
    {
        if (mouseLeftPressed)
        {
            if (options.enableMusic)
            {
                sound = new sf::Sound();
                sound->setBuffer(*buffer);
                sf::Time startTime = sf::seconds(debug_startTime);
                sound->setPlayingOffset(startTime);
                sound->play();
            }
            totalElapsedTime = debug_startTime;
            gameElapsedTime  = debug_startTime;
            hasStarted       = true;
        }

        ballPosition.x = ballMinX + (1 - padPositionX) * (ballMaxX - ballMinX);
        ballPosition.y = ballBottomY;
        ballPosition.z = ballMinZ + (1 - padPositionZ) * ((ballMaxZ + cameraWallOffset) - ballMinZ);
    }
    else
    {
        totalElapsedTime += timeDelta;
        if (hasLost)
        {
            if (mouseLeftReleased)
            {
                hasLost          = false;
                hasStarted       = false;
                currentKeyFrame  = 0;
                previousKeyFrame = 0;
            }
        }
        else if (isPaused)
        {
            if (mouseRightReleased)
            {
                isPaused = false;
                if (options.enableMusic)
                {
                    sound->play();
                }
            }
        }
        else
        {
            gameElapsedTime += timeDelta;
            if (mouseRightReleased)
            {
                isPaused = true;
                if (options.enableMusic)
                {
                    sound->pause();
                }
            }
            // Get the timing for the beat of the song
            for (unsigned int i = currentKeyFrame; i < keyFrameTimeStamps.size(); i++)
            {
                if (gameElapsedTime < keyFrameTimeStamps.at(i))
                {
                    continue;
                }
                currentKeyFrame = i;
            }

            jumpedToNextFrame = currentKeyFrame != previousKeyFrame;
            previousKeyFrame  = currentKeyFrame;

            double frameStart = keyFrameTimeStamps.at(currentKeyFrame);
            double frameEnd   = keyFrameTimeStamps.at(currentKeyFrame + 1); // Assumes last keyframe at infinity

            double elapsedTimeInFrame    = gameElapsedTime - frameStart;
            double frameDuration         = frameEnd - frameStart;
            double fractionFrameComplete = elapsedTimeInFrame / frameDuration;

            KeyFrameAction currentOrigin      = keyFrameDirections.at(currentKeyFrame);
            KeyFrameAction currentDestination = keyFrameDirections.at(currentKeyFrame + 1);

            // Synchronize ball with music
            double ballYCoord;
            if (currentOrigin == BOTTOM && currentDestination == BOTTOM)
                ballYCoord = ballBottomY;
            else if (currentOrigin == TOP && currentDestination == TOP)
                ballYCoord = ballBottomY + BallVerticalTravelDistance;
            else if (currentDestination == BOTTOM)
                ballYCoord = ballBottomY + BallVerticalTravelDistance * (1 - fractionFrameComplete);
            else if (currentDestination == TOP)
                ballYCoord = ballBottomY + BallVerticalTravelDistance * fractionFrameComplete;

            // Make ball move
            const float ballSpeed = 60.0f;
            ballPosition.x += timeDelta * ballSpeed * ballDirection.x;
            ballPosition.y = ballYCoord;
            ballPosition.z += timeDelta * ballSpeed * ballDirection.z;

            // Make ball bounce
            if (ballPosition.x < ballMinX)
            {
                ballPosition.x = ballMinX;
                ballDirection.x *= -1;
            }
            else if (ballPosition.x > ballMaxX)
            {
                ballPosition.x = ballMaxX;
                ballDirection.x *= -1;
            }
            if (ballPosition.z < ballMinZ)
            {
                ballPosition.z = ballMinZ;
                ballDirection.z *= -1;
            }
            else if (ballPosition.z > ballMaxZ)
            {
                ballPosition.z = ballMaxZ;
                ballDirection.z *= -1;
            }

            if (options.enableAutoplay)
            {
                padPositionX = 1 - (ballPosition.x - ballMinX) / (ballMaxX - ballMinX);
                padPositionZ = 1 - (ballPosition.z - ballMinZ) / ((ballMaxZ + cameraWallOffset) - ballMinZ);
            }

            // Check if the ball is hitting the pad when the ball is at the bottom.
            // If not, you just lost the game! (hehe)
            if (jumpedToNextFrame && currentOrigin == BOTTOM && currentDestination == TOP)
            {
                double padLeftX  = box->position.x - (boxDimensions.x / 2) + (1 - padPositionX) * (boxDimensions.x - padDimensions.x);
                double padRightX = padLeftX + padDimensions.x;
                double padFrontZ = box->position.z - (boxDimensions.z / 2) + (1 - padPositionZ) * (boxDimensions.z - padDimensions.z);
                double padBackZ  = padFrontZ + padDimensions.z;

                if (ballPosition.x < padLeftX
                    || ballPosition.x > padRightX
                    || ballPosition.z < padFrontZ
                    || ballPosition.z > padBackZ)
                {
                    hasLost = true;
                    if (options.enableMusic)
                    {
                        sound->stop();
                        delete sound;
                    }
                }
            }
        }
    }

    ball->position = ballPosition;
    ball->rotation = { 0, totalElapsedTime * 2, 0 };

    lights[0]->position.x = ball->position.x;
    lights[0]->position.z = ball->position.z;

    pad->position = {
        box->position.x - (boxDimensions.x / 2) + (padDimensions.x / 2) + (1 - padPositionX) * (boxDimensions.x - padDimensions.x),
        box->position.y - (boxDimensions.y / 2) + (padDimensions.y / 2),
        box->position.z - (boxDimensions.z / 2) + (padDimensions.z / 2) + (1 - padPositionZ) * (boxDimensions.z - padDimensions.z)
    };
}
