#include "scene.hpp"

#include <chrono>
#include <ctime>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

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

int shapeAppearanceIndex = 0;
int bustAppearanceIndex  = 0;

const std::vector<AppearanceType> SHAPE_APPEARANCES = {
    CLASSIC,
    REFLECTIVE,
    REFRACTIVE,
};

const std::vector<AppearanceType> BUST_APPEARANCES = {
    TEXTURED,
    REFLECTIVE,
    REFRACTIVE,
};

bool rotateBust = true;



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
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        bustAppearanceIndex = (bustAppearanceIndex + 1) % BUST_APPEARANCES.size();
        bust->appearance    = BUST_APPEARANCES[bustAppearanceIndex];
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        shapeAppearanceIndex = (shapeAppearanceIndex + 1) % SHAPE_APPEARANCES.size();
        for (SceneNode *node : shapes->children) node->appearance = SHAPE_APPEARANCES[shapeAppearanceIndex];
    }
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

    shaderManager = new ShaderManager();
    skyboxManager = new SkyboxManager(new Shader("skybox.vert", "skybox.frag"));
    keyboard      = new Keyboard();
    camera        = new Camera(glm::vec3(0, 0, 30));

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
    float spacing = size * 1.5f;

    shapes              = new SceneNode();
    SceneNode *pyramid  = SceneNode::fromMesh(SHAPES::Pyramid(size, size), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *cube1    = SceneNode::fromMesh(SHAPES::Cube(size), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *cube2    = SceneNode::fromMesh(SHAPES::Cube(size), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *sphere1  = SceneNode::fromMesh(SHAPES::Sphere(size / 2), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *sphere2  = SceneNode::fromMesh(SHAPES::Sphere(size / 2), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *cylinder = SceneNode::fromMesh(SHAPES::Cylinder(size / 2, size), SHAPE_APPEARANCES[shapeAppearanceIndex]);

    pyramid->translate(-spacing, 0, 0);
    sphere1->translate(0, 0, 0);
    cube1->translate(spacing, 0, 0);
    sphere2->translate(-spacing, 0, -spacing);
    cylinder->translate(0, 0, -spacing);
    cube2->translate(spacing, 0, -spacing);

    root->addChild(shapes);
    shapes->addChild(pyramid);
    shapes->addChild(cube1);
    shapes->addChild(cube2);
    shapes->addChild(sphere1);
    shapes->addChild(sphere2);
    shapes->addChild(cylinder);

    // Rotating Bust
    std::string resolution = "1k";
    if (OPTIONS::mode == OPTIONS::DEMO) resolution = "4k";
    bust = SceneNode::fromModelName("marble_bust", resolution, BUST_APPEARANCES[bustAppearanceIndex]);

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
    glm::mat4 projection = UTILS::getPerspectiveMatrix(camera->FOV);
    glm::mat4 view       = UTILS::getViewMatrix(camera->position, camera->front, camera->up);
    // Activate correct framebuffer
    Framebuffer::activateScreen();
    // Render The scene
    skyboxManager->render(view, projection);
    for (SceneNode *node : root->getAllChildren()) renderNode(node, view, projection, camera->position);
}



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
    for (SceneNode *node : root->getAllChildren())
    {
        if (node->appearance == REFLECTIVE || node->appearance == REFRACTIVE)
        {
            node->environmentBuffer->activate();

            for (unsigned int side = 0; side < 6; side++)
            {
                glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0); // 90 degrees is supposed to make the cubemap faces line up perfectly
                glm::mat4 view       = UTILS::getViewMatrix(node->position, CubemapDirections::view[side], CubemapDirections::up[side]);

                node->environmentBuffer->selectRenderTargetSide(side);

                // Render Scene
                skyboxManager->render(view, projection);
                for (SceneNode *graphNode : root->getAllChildren())
                {
                    // Render all nodes except this node
                    if (graphNode != node) renderNode(graphNode, view, projection, node->position);
                }
            }
            node->hasEnvironmentMap = true;
        }
    }
}



/**
 * @brief Render the given node with the given view, projection and camera position.
 * The method sets all uniform values and renders the node's mesh.
 *
 * To make things a easy for myself im ditching the recursive rendering of the scenenodes.
 * This enables me to render the entire scene, but treat a specific node in a particular way (e.g skip it)
 * I have to do this to prevent the node trying to sample itself when creating the environment map, which creates ugly artifacts.
 * The ideal way to solve this would have the node be able to reflect itself, but i dont have time figure that out :/
 *
 * @param node
 * @param view
 * @param projection
 * @param cameraPos
 */
void renderNode(
    SceneNode *node,
    glm::mat4 view,
    glm::mat4 projection,
    glm::vec3 cameraPosition)
{
    // get correct shader for this node
    Shader *shader = shaderManager->getShaderFor(node);
    if (shader == nullptr) return;

    shader->activate();

    shader->setUniform(UNIFORMS::M, node->M);
    shader->setUniform(UNIFORMS::V, view);
    shader->setUniform(UNIFORMS::P, projection);
    shader->setUniform(UNIFORMS::N, node->N);
    shader->setUniform(UNIFORMS::camera_position, cameraPosition);
    shader->setUniform(UNIFORMS::sunlight_color, skyboxManager->getSunlightColor());
    shader->setUniform(UNIFORMS::sunlight_direction, skyboxManager->getSunlightDirection());

    // let the shader know if it should use textures or not
    shader->setUniform(UNIFORMS::has_textures, node->textures.hasTextures);
    if (node->textures.hasTextures)
    {
        glBindTextureUnit(BINDINGS::diffuse_map, node->textures.diffuseID);
        glBindTextureUnit(BINDINGS::normal_map, node->textures.normalID);
        glBindTextureUnit(BINDINGS::roughness_map, node->textures.roughnessID);
    }

    // Set correct skybox depending upon wether this ndoe has a dynamic cubemap or not
    if (node->hasEnvironmentMap)
        glBindTextureUnit(BINDINGS::skybox, node->environmentBuffer->textureID);
    else
        glBindTextureUnit(BINDINGS::skybox, skyboxManager->getTextureID());

    // Finally render the nodes mesh
    node->render();
}