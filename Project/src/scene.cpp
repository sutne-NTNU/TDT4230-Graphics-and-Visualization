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

Framebuffer *backsideNormalBuffer;

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

float bustRotationSpeed = 0.0f;



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
    if (key == GLFW_KEY_T && action == GLFW_PRESS) bustRotationSpeed = bustRotationSpeed == 15 ? 0 : 15;
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
}

/** Called everytime the cursor changes place */
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    camera->handleCursorPosition((float)xpos, (float)ypos);
}



/** Executed once before main loop */
void initScene(GLFWwindow *window)
{
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyboardCallback);

    shaderManager        = new ShaderManager();
    skyboxManager        = new SkyboxManager(new Shader("skybox.vert", "skybox.frag"));
    keyboard             = new Keyboard();
    camera               = new Camera(glm::vec3(0, 0, 30));
    backsideNormalBuffer = new Framebuffer(WINDOW::width, WINDOW::height);

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
    float size    = 10;
    float spacing = size * 1.5;

    shapes              = new SceneNode();
    SceneNode *pyramid  = SceneNode::fromMesh(SHAPES::Pyramid(size, size), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *cube     = SceneNode::fromMesh(SHAPES::Cube(size), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *sphere   = SceneNode::fromMesh(SHAPES::Sphere(size / 2), SHAPE_APPEARANCES[shapeAppearanceIndex]);
    SceneNode *cylinder = SceneNode::fromMesh(SHAPES::Cylinder(size / 2, size), SHAPE_APPEARANCES[shapeAppearanceIndex]);

    pyramid->position.x  = spacing * -1;
    cube->position.x     = spacing * 1;
    sphere->position.x   = spacing * 0;
    cylinder->position.x = spacing * 2;

    root->addChild(shapes);
    shapes->addChild(pyramid);
    shapes->addChild(cube);
    shapes->addChild(sphere);
    // shapes->addChild(cylinder);

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
    camera->updatePosition(deltaTime, keyboard->keysCurrentlyPressed);
    bust->rotate(0, deltaTime * bustRotationSpeed, 0);

    root->updateTransformations(glm::mat4(1));
}


void demoRender()
{
    renderFrame();
}
/**
 * @brief Called after the scene has been updated, renders the entier scene
 */
void renderFrame()
{
    // First we need to get accurate reflections and refractions for this frame for each node that needs it
    for (SceneNode *node : root->getAllChildren())
    {
        if (node->appearance == REFLECTIVE || node->appearance == REFRACTIVE)
        {
            fillReflectionBuffer(node);
        }
    }

    // then we update the backside normals used for double refraction
    updateBackSideNormals();

    // Once all buffers are filled, we can render the scene from the actual cameras perspective
    glm::mat4 projection = UTILS::getPerspectiveMatrix(60);
    glm::mat4 view       = UTILS::getViewMatrix(camera->position, camera->front, camera->up);
    // Activate correct framebuffer
    Framebuffer::activateScreen();
    // Render The scene
    skyboxManager->render(view, projection);
    renderSceneGraph(view, projection, camera->position);
}



/**
 * @brief Renders the entire scene from the nodes perspective and stores it
 * in the given node's dynamic cubemap for reflections
 */
void fillReflectionBuffer(SceneNode *node)
{
    node->environmentBuffer->activate();

    for (unsigned int side = 0; side < 6; side++)
    {
        glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0);
        glm::mat4 view       = UTILS::getViewMatrix(node->position, CubemapDirections::view[side], CubemapDirections::up[side]);

        node->environmentBuffer->selectRenderTargetSide(side);

        // Render Scene
        skyboxManager->render(view, projection);
        renderSceneGraph(view, projection, node->position, node, SKIP);
    }
    node->hasEnvironmentMap = true;
}



/**
 * @brief Renders the entire scene, but switches to front-face culling for this node to get accurate refractions for
 * when light leaves the object.
 */
void updateBackSideNormals()
{
    // backsideNormalBuffer->activate();
    Framebuffer::activateScreen();

    // Same matrices as the camera
    glm::mat4 projection = UTILS::getPerspectiveMatrix(60);
    glm::mat4 view       = UTILS::getViewMatrix(camera->position, camera->front, camera->up);


    glCullFace(GL_FRONT);
    for (SceneNode *node : root->getAllChildren())
    {
        if (node->appearance == REFRACTIVE)
        {
            renderNode(node, view, projection, camera->position, true);
        }
    }
    glCullFace(GL_BACK);
}



/**
 * @brief Render the given node with the given view, projection and camera position.
 * The method sets all uniform values and renders the node's mesh.
 *
 * @param node
 * @param view
 * @param projection
 * @param cameraPos
 * @param flipNormals inverts the normals in the fragment shader (for exit refraction)
 */
void renderNode(
    SceneNode *node,
    glm::mat4 view,
    glm::mat4 projection,
    glm::vec3 cameraPos,
    bool backSidePass)
{
    // get correct shader for this node
    Shader *shader = shaderManager->getShaderFor(node);
    if (shader == nullptr) return;

    shader->activate();

    shader->setUniform(UNIFORMS::M, node->M);
    shader->setUniform(UNIFORMS::V, view);
    shader->setUniform(UNIFORMS::P, projection);
    shader->setUniform(UNIFORMS::N, node->N);
    shader->setUniform(UNIFORMS::camera_position, cameraPos);
    shader->setUniform(UNIFORMS::sunlight_color, skyboxManager->getSunColor());
    shader->setUniform(UNIFORMS::sunlight_direction, skyboxManager->getSunDirection());
    shader->setUniform(UNIFORMS::back_side_pass, backSidePass);

    // let the shader know if it should use textures or not
    shader->setUniform(UNIFORMS::has_textures, node->textures.hasTextures);
    if (node->textures.hasTextures)
    {
        glBindTextureUnit(BINDINGS::texture_map, node->textures.colorID);
        glBindTextureUnit(BINDINGS::normal_map, node->textures.normalID);
        glBindTextureUnit(BINDINGS::roughness_map, node->textures.roughnessID);
    }

    // Set correct skybox depending upon wether this ndoe has a dynamic cubemap or not
    glBindTextureUnit(BINDINGS::skybox, skyboxManager->getTextureID());
    if (node->hasEnvironmentMap) glBindTextureUnit(BINDINGS::skybox, node->environmentBuffer->textureID);

    // Set backside normals for refractive objects
    glBindTextureUnit(BINDINGS::back_side_normal_map, backsideNormalBuffer->textureID);

    // Finally render the nodes mesh
    node->render();
}



/**
 * @brief To make things a littel easier for myself im dithing the recursive rendering of the scenenodes.
 * This enables me to render the entire scene, but treat a specific node in a particular way (e.g skip it, reverse it etc.);
 *
 * @param specialNode Node to treat differently than the rest of the scenegraph
 * @param specialCase How should this special node be treated
 */
void renderSceneGraph(
    glm::mat4 view,
    glm::mat4 projection,
    glm::vec3 cameraPosition,
    SceneNode *specialNode,
    SpecialRenderingCase specialCase)
{
    if (specialCase == NONE)
    {
        for (SceneNode *node : root->getAllChildren()) renderNode(node, view, projection, cameraPosition);
    }

    if (specialCase == SKIP)
    {
        for (SceneNode *node : root->getAllChildren())
        {
            if (node != specialNode) renderNode(node, view, projection, cameraPosition);
        }
    }
}
