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

float bustRotationSpeed = 15;



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
        shapes->appearance   = SHAPE_APPEARANCES[shapeAppearanceIndex];
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

    shaderManager = new ShaderManager();
    skyboxManager = new SkyboxManager(new Shader("skybox.vert", "skybox.frag"));
    keyboard      = new Keyboard();
    camera        = new Camera(glm::vec3(0, 10, 50), -10.0);

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
    float size    = 6;
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

    // Build SceneGraph
    root->addChild(shapes);
    shapes->addChild(pyramid);
    shapes->addChild(cube);
    shapes->addChild(sphere);
    // shapes->addChild(cylinder);

    // Rotating Bust
    std::string resolution = "1k";
    if (OPTIONS::mode == OPTIONS::DEMO) resolution = "4k";
    bust = SceneNode::fromModelName("marble_bust", resolution);

    bust->setScale(100);
    bust->translate(0, -25, 100);
    bust->rotate(0, 180, 0);

    root->addChild(bust);
}



/** Executed At start of each new frame to update all positions and states */
void updateState(GLFWwindow *window, float deltaTime)
{
    camera->updatePosition(deltaTime, keyboard->keysCurrentlyPressed);
    bust->rotate(0, deltaTime * bustRotationSpeed, 0);
}



/** Executed after the frame state has been updated */
void renderFrame(GLFWwindow *window)
{
    // renderRefractionStep(window);
    // renderReflectionStep(window);
    renderFinal(window);
    // renderContentsOf(framebuffers->refractionFramebuffer);
}

void renderNode(SceneNode *node, glm::mat4 view, glm::mat4 projection, glm::vec3 cameraPos = camera->position)
{
    // get correct shader for this node
    Shader *shader = shaderManager->getShaderFor(node);
    if (shader != nullptr)
    {
        shader->setUniform(UNIFORMS::M, node->M);
        shader->setUniform(UNIFORMS::V, view);
        shader->setUniform(UNIFORMS::P, projection);
        shader->setUniform(UNIFORMS::N, node->N);
        shader->setUniform(UNIFORMS::camera_position, cameraPos);
        shader->setUniform(UNIFORMS::sunlight_color, skyboxManager->getSunColor());
        shader->setUniform(UNIFORMS::sunlight_direction, skyboxManager->getSunDirection());

        // let the shader know if it can use textures or not
        if (node->textures.hasTextures)
        {
            shader->setUniform(UNIFORMS::has_textures, true);
            glBindTextureUnit(BINDINGS::texture_map, node->textures.colorID);
            glBindTextureUnit(BINDINGS::normal_map, node->textures.normalID);
            glBindTextureUnit(BINDINGS::roughness_map, node->textures.roughnessID);
        }
        else
        {
            shader->setUniform(UNIFORMS::has_textures, true);
        }
        // Finally render the node
        node->render();
    }
    for (SceneNode *child : node->children) renderNode(child, view, projection, cameraPos);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lots of duplicate code here, but it's a lot easier to read what happens in each render-pass like this   //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * For the "refraction" render-pass, we render the scene from the camera's point of view, but we are only
 * going to draw the normals of the "backside" of opaque objects. This will be saved to a texture and used later
 * to create the double refraction effect.
 */
void renderRefractionStep(GLFWwindow *window)
{
    // // Calculate ViewProjection Matrix
    // glm::mat4 projection = UTILS::getPerspectiveMatrix(90);
    // glm::mat4 view       = camera->getViewMatrix();

    // // Update Transformation for SceneGraph
    // root->updateTransformations(glm::mat4(1), projection * view);

    // shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::REFRACTION);
    // shader->setUniform("camera.position", camera->getPosition());

    // glCullFace(GL_FRONT);
    // root->render(shader);
    // glCullFace(GL_BACK);
    // glBindTextureUnit(11, framebuffers->refractionFramebuffer.textureID);
}


/**
 * For reflections the scene is rendered 6 times (at full scale) from the center of an object,
 * we then create a cubemap from these 6 textures that can bused to sample the reflections from
 * in the final render pass.
 */
void renderReflectionStep(GLFWwindow *window)
{
    // for (SceneNode *node : root->getAllChildren())
    // {
    //     if (node->appearance.reflectivity == 0) continue;
    SceneNode *node = root;
    // Activate this nodes reflection framebuffer
    node->reflectionBuffer->activate();
    for (unsigned int side = 0; side < 6; side++)
    {
        // Update all transformations
        glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0);
        glm::mat4 view       = UTILS::getViewMatrix(node->position, CubemapDirections::view[side], CubemapDirections::up[side]);

        skyboxManager->updateMatrices(view, projection);
        root->updateTransformations(glm::mat4(1));

        // Render to correct face of the cubemap
        node->reflectionBuffer->selectRenderTargetSide(side);

        // Render Scene
        skyboxManager->render();
        renderNode(root, view, projection, node->position);
    }
    glBindTextureUnit(BINDINGS::skybox, node->reflectionBuffer->textureID);
    // }
}



/**
 * The final render pass of the frame, using the textures we have created we can now
 * create a quite realistic "ray-traced" scene.
 */
void renderFinal(GLFWwindow *window)
{
    // Update all transformations and matrices
    glm::mat4 projection = UTILS::getPerspectiveMatrix(60);
    glm::mat4 view       = UTILS::getViewMatrix(camera->position, camera->front, camera->up);

    skyboxManager->updateMatrices(view, projection);
    root->updateTransformations(glm::mat4(1));

    // Activate correct framebuffer
    Framebuffer::activateScreen();

    // Render Scene
    skyboxManager->render();
    renderNode(root, view, projection);
}