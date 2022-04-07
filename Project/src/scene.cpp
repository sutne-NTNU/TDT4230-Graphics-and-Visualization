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
#include "managers/skyboxManager.hpp"
#include "options.hpp"
#include "structs/appearance_struct.hpp"
#include "utilities/shapes.hpp"
#include "utilities/utils.hpp"



Gloom::Shader *shader;
Camera *camera;
Keyboard *keyboard;
SkyboxManager *skyboxManager;

// Nodes that are updated after init
SceneNode *root;
SceneNode *shapes;

SceneNode *bust;
const std::vector<Appearance> BUST_APPEARANCES = {
    TEXTURE,
    GLASS,
    CHROME,
};
int bustAppearanceIndex = 0;
float rotationSpeed     = 15;



/** Called every time a key state changes on the keyboard */
void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    // Update pressed keys, used for smooth movement
    keyboard->handleKeyAction(key, action);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_L && action == GLFW_PRESS) skyboxManager->swapCubemap();
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) camera->decreaseSensitivity();
    if (key == GLFW_KEY_E && action == GLFW_PRESS) camera->increaseSensitivity();
    if (key == GLFW_KEY_R && action == GLFW_PRESS) camera->resetSensitivity();
    if (key == GLFW_KEY_T && action == GLFW_PRESS) rotationSpeed = rotationSpeed == 15 ? 0 : 15;
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        bustAppearanceIndex = (bustAppearanceIndex + 1) % BUST_APPEARANCES.size();
        bust->appearance.setTo(BUST_APPEARANCES[bustAppearanceIndex]);
    }
    if (key == GLFW_KEY_X && action == GLFW_PRESS) UTILS::takeScreenshot(window);
    // Debugging
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        std::cout << "Sun -> Camera: " << glm::to_string(camera->getVectorIntoCamera()) << std::endl;
        std::cout << "Camera:\n\tPos:" << glm::to_string(camera->position)
                  << "\n\tPitch: " << camera->pitch
                  << "\n\tYaw: " << camera->yaw << std::endl;
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

    shader = new Gloom::Shader("main.vert", "main.frag");
    shader->activate();

    skyboxManager = new SkyboxManager();
    keyboard      = new Keyboard();
    camera        = new Camera(glm::vec3(0, 10, 50), -10.0);

    // Create And Inititalize Nodes and SceneGraph
    root = new SceneNode();
    initSceneGraph();
    if (OPTIONS::verbose) printf("Initilized scene with %d nodes\n", root->getNumChildren());
}



/** Give all objects and camera their dimensions and starting positions/directions */
void initSceneGraph()
{
    // Construct Objects
    float size = 6;
    shapes     = new SceneNode();
    // SceneNode *prism    = SceneNode::fromMesh(SHAPES::Prism(size, size, size), CHROME);
    SceneNode *pyramid  = SceneNode::fromMesh(SHAPES::Pyramid(size, size), MATTE_RED);
    SceneNode *cube     = SceneNode::fromMesh(SHAPES::Cube(size), MATTE_BLUE);
    SceneNode *sphere   = SceneNode::fromMesh(SHAPES::Sphere(size / 2), CHROME);
    SceneNode *cylinder = SceneNode::fromMesh(SHAPES::Cylinder(size / 2, size), MATTE_BLUE);

    std::string resolution = "1k";
    if (OPTIONS::mode == OPTIONS::DEMO) resolution = "4k";
    bust = SceneNode::fromModelName("marble_bust", resolution);

    // Build SceneGraph
    root->addChild(shapes);
    shapes->addChild(pyramid);
    shapes->addChild(cube);
    shapes->addChild(sphere);
    root->addChild(bust);

    // Place Objects in their initial positions
    // Coordinate system is as below (based on initial camera position):
    //
    //         +y -z
    //          | /
    //          |/
    //    -x----|----+x
    //         /|
    //        / |
    //      +z  -y

    float spacing = size * 1.5;

    // prism->position.x    = spacing * -2;
    pyramid->position.x  = spacing * -1;
    cube->position.x     = spacing * 1;
    sphere->position.x   = spacing * 0;
    cylinder->position.x = spacing * 2;

    // prism->rotate(90);
    bust->setScale(100);
    bust->translate(0, -25, 150);
    bust->rotate(0, 180, 0);
}



/** Executed At start of each new frame to update all positions and states */
void updateState(GLFWwindow *window, float deltaTime)
{
    // Find updated cameraposition
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    camera->updatePosition(deltaTime, keyboard->keysCurrentlyPressed);
    bust->rotate(0, deltaTime * rotationSpeed, 0);
}



/** Executed after the frame state has been updated */
void renderFrame(GLFWwindow *window)
{
    // renderRefractionStep(window);
    renderReflectionStep(window);
    renderFinal(window);
    // renderContentsOf(framebuffers->refractionFramebuffer);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sorry about the duplicate code here, but it's a lot easier to read what happens in each render-pass like this   //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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
    for (SceneNode *node : root->getAllChildren())
    {
        if (node->appearance.reflectivity == 0) continue;

        node->reflectionBuffer->activate();
        for (unsigned int side = 0; side < 6; side++)
        {
            node->reflectionBuffer->selectRenderTargetSide(side);

            glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0);
            glm::mat4 view       = UTILS::getViewMatrix(node->position, CubemapDirections::view[side], CubemapDirections::up[side]);

            skyboxManager->updateVP(view, projection);
            root->updateTransformations(glm::mat4(1), projection * view);

            skyboxManager->render(shader);

            shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::REFLECTION);
            shader->setUniform("camera.position", node->position);
            shader->setUniform("sun.color", skyboxManager->getSunColor());
            shader->setUniform("sun.direction", skyboxManager->getSunDirection());

            root->render(shader);
        }
        glBindTextureUnit(BINDINGS::reflection_cubemap, node->reflectionBuffer->textureID);
    }
}



/**
 * The final render pass of the frame, using the textures we have created we can now
 * create a quite realistic "ray-traced" scene.
 */
void renderFinal(GLFWwindow *window)
{
    // Update all transformation matrices
    glm::mat4 projection = UTILS::getPerspectiveMatrix(60);
    glm::mat4 view       = camera->getViewMatrix();

    skyboxManager->updateVP(view, projection);
    root->updateTransformations(glm::mat4(1), projection * view);

    // Activate correct framebuffer
    Framebuffer::activateScreen();
    shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::RENDER);

    // Render the rest of the scene
    shader->setUniform("camera.position", camera->getPosition());
    shader->setUniform("sun.color", skyboxManager->getSunColor());
    shader->setUniform("sun.direction", skyboxManager->getSunDirection());
    root->render(shader);
}