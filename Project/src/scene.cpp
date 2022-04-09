#include "scene.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "classes/camera.hpp"
#include "classes/framebuffer.hpp"
#include "classes/image.hpp"
#include "classes/keyboard.hpp"
#include "classes/mesh.hpp"
#include "classes/sceneNode.hpp"
#include "classes/shader.hpp"
#include "managers/shaderManager.hpp"
#include "managers/skyboxManager.hpp"
#include "options.hpp"
#include "utilities/shapes.hpp"
#include "utilities/utils.hpp"



Camera *camera;
Keyboard *keyboard;
SkyboxManager *skyboxManager;
ShaderManager *shaderManager;

SceneNode *root;
SceneNode *shapes;
SceneNode *bust;

bool rotateBust = false;



/** Called every time a key state changes on the keyboard */
void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    // Update pressed keys, used for smooth movement of camera
    keyboard->handleKeyAction(key, action); // WASD, LShift, Space
    // Handle other key presses
    if (key == GLFW_KEY_X && action == GLFW_PRESS) UTILS::takeScreenshot(window);
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) camera->decreaseSensitivity();
    if (key == GLFW_KEY_E && action == GLFW_PRESS) camera->increaseSensitivity();
    if (key == GLFW_KEY_R && action == GLFW_PRESS) camera->resetSensitivity();
    if (key == GLFW_KEY_L && action == GLFW_PRESS) skyboxManager->swapSkybox();
    if (key == GLFW_KEY_T && action == GLFW_PRESS) rotateBust = !rotateBust;
    if (key == GLFW_KEY_M && action == GLFW_PRESS) bust->swapAppearance();
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        for (SceneNode *node : shapes->children) node->swapAppearance();
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        camera->yaw += 90;
        camera->updateCameraViewVectors();
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        for (SceneNode *node : root->getAllChildren()) node->increaseEnvironmentResolution();
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        for (SceneNode *node : root->getAllChildren()) node->decreaseEnvironmentResolution();
}

/** Called everytime the cursor changes place */
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    camera->handleCursorPosition((float)xpos, (float)ypos);
}

void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera->handleScroll((float)yoffset);
}



/** Executed once before main loop */
void initScene(GLFWwindow *window)
{
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetKeyCallback(window, keyboardCallback);

    keyboard      = new Keyboard();
    camera        = new Camera(glm::vec3(0, 0, 30));
    shaderManager = new ShaderManager();
    skyboxManager = new SkyboxManager();

    // Create And Inititalize Nodes and SceneGraph
    root = new SceneNode();
    initSceneGraph();
    if (OPTIONS::verbose) printf("Initilized scene with %d nodes\n", root->getNumChildren());
}



/** Give all objects their dimensions and start positions */
void initSceneGraph()
{
    // Coordinate system is as below (based on initial camera position):
    //
    //         +y -z
    //          | /
    //          |/
    //    -x----|----+x
    //         /|
    //        / |
    //      +z  -y

    // Geometric Shapes
    float size    = 10.0f;
    float spacing = size * 2.0f;

    shapes              = new SceneNode();
    SceneNode *prism    = SceneNode::fromMesh(SHAPES::Prism(size, size, size), REFLECTIVE);
    SceneNode *pyramid  = SceneNode::fromMesh(SHAPES::Pyramid(size / 3, size / 3), SUNLIT);
    SceneNode *cube     = SceneNode::fromMesh(SHAPES::Cube(size), REFLECTIVE);
    SceneNode *sphere1  = SceneNode::fromMesh(SHAPES::Sphere(size / 2), REFRACTIVE);
    SceneNode *cylinder = SceneNode::fromMesh(SHAPES::Cylinder(size / 2, size), REFRACTIVE);
    SceneNode *sphere2  = SceneNode::fromMesh(SHAPES::Sphere(size / 2), REFLECTIVE);

    prism->rotate(90);
    prism->translate(-spacing, 0, -spacing);
    pyramid->translate(0, 0, -spacing);
    cube->translate(spacing, 0, -spacing);
    sphere1->translate(-spacing, 0, 0);
    cylinder->translate(0, 0, 0);
    sphere2->translate(spacing, 0, 0);

    root->addChild(shapes);
    shapes->addChild(prism);
    shapes->addChild(pyramid);
    shapes->addChild(cube);
    shapes->addChild(sphere1);
    shapes->addChild(cylinder);
    shapes->addChild(sphere2);

    // Rotating Bust
    std::string resolution = "1k";
    if (OPTIONS::mode == OPTIONS::DEMO) resolution = "4k";
    bust = SceneNode::fromModelName("marble_bust", resolution, SUNLIT);

    bust->setScale(100);
    bust->translate(0, -25, 85);
    bust->rotate(0, 180, 0);
    root->addChild(bust);
}



/**
 * @brief Called before renderFrame(), and updates all objects in the scene
 *
 * @param deltaTime time (in seconds) since the previous frame was updated
 */
void updateState(float deltaTime)
{
    // Update all positions and states
    camera->updatePosition(deltaTime, keyboard->keysCurrentlyPressed);
    if (rotateBust) bust->rotate(0, deltaTime * 15.0f, 0);

    // Update all transformations to match the new positions
    root->updateTransformations(glm::mat4(1));
}



/**
 * @brief Called after the scene has been updated, renders the entier scene
 */
void renderFrame()
{
    // First we need to get accurate reflections and refractions for the nodes that need it
    updateEnvironmentBuffers();

    // Once all buffers are filled, we can render the scene from the cameras perspective
    glm::mat4 projection = UTILS::getPerspectiveMatrix(camera->FOV, float(WINDOW::width) / float(WINDOW::height));
    glm::mat4 view       = UTILS::getViewMatrix(camera->position, camera->front, camera->up);
    // Activate correct framebuffer
    Framebuffer::activateScreen();
    // Render The scene
    skyboxManager->render(view, projection);
    for (SceneNode *node : root->getAllChildren())
    {
        renderNode(node, view, projection, camera->position, shaderManager->getShaderFor(node));
    }
}



// void renderFrame()
// {
//     Framebuffer::activateScreen();
//     SceneNode *masterNode = shapes->children[4];

//     // int side            = 4;
//     // glm::vec3 direction = CubemapDirections::view[side];
//     // glm::vec3 up        = CubemapDirections::up[side];
//     // glm::vec3 position  = masterNode->position;

//     glm::vec3 direction = camera->front;
//     glm::vec3 up        = camera->up;
//     glm::vec3 position  = camera->position;

//     glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0f, 1.0f);
//     glm::mat4 view       = UTILS::getViewMatrix(position, direction, up);

//     glViewport(0, 0, OPTIONS::environmentBufferResolution, OPTIONS::environmentBufferResolution);

//     // Render Scene, but skip this node
//     skyboxManager->render(view, projection);
//     for (SceneNode *node : root->getAllChildren())
//     {
//         if (node == masterNode) continue;
//         renderNode(node, view, projection, masterNode->position, shaderManager->getShaderFor(node));
//     }
// }


/**
 * @brief Renders the entire scene from the nodes perspective and stores it
 * in the given node's dynamic cubemap for use in reflections and refractions
 *
 * The great thing about each node storing their own cubemap is that they
 * keep that when the different nodes create their updated ones. In essence
 * i have created ray tracing with infinite ray bounces ;)
 */
void updateEnvironmentBuffers()
{
    for (SceneNode *masterNode : root->getAllChildren())
    {
        // Make sure node actually needs the environment map
        if (masterNode->appearance != REFLECTIVE && masterNode->appearance != REFRACTIVE) continue;

        masterNode->environmentBuffer->activate();
        for (unsigned int side = 0; side < 6; side++)
        {
            glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0f, 1.0f);
            glm::mat4 view       = UTILS::getViewMatrix(masterNode->position, CubemapDirections::view[side], CubemapDirections::up[side]);

            masterNode->environmentBuffer->selectRenderTargetSide(side);

            // Render Scene, but skip this node
            skyboxManager->render(view, projection);
            for (SceneNode *node : root->getAllChildren())
            {
                if (node == masterNode) continue;
                renderNode(node, view, projection, masterNode->position, shaderManager->getShaderFor(node));
            }
        }
        masterNode->hasEnvironmentMap = true;
    }
}


/**
 * @brief Activates the shader. Passes all scene information to the shader and renders the node
 *
 * @param node the node to render
 * @param view View Matrix
 * @param projection Projection Matrix
 * @param cameraPosition The position of which the nodes should be seen from
 * @param shader The shader to use (can be nullptr)
 */
void renderNode(SceneNode *node, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPosition, Shader *shader)
{
    if (shader == nullptr) return;
    shader->activate();

    shader->setUniform(UNIFORMS::V, view);
    shader->setUniform(UNIFORMS::P, projection);
    shader->setUniform(UNIFORMS::camera_position, cameraPosition);
    shader->setUniform(UNIFORMS::sunlight_color, skyboxManager->getSunlightColor());
    shader->setUniform(UNIFORMS::sunlight_direction, skyboxManager->getSunlightDirection());
    glBindTextureUnit(BINDINGS::skybox, skyboxManager->getTextureID());

    node->render(shader);
}