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
#include <glm/vec3.hpp>

#include "classes/camera.hpp"
#include "classes/framebufferHandler.hpp"
#include "classes/keyboard.hpp"
#include "classes/sceneNode.hpp"
#include "classes/shader.hpp"
#include "classes/skyboxHandler.hpp"
#include "options.hpp"
#include "structs/appearance_struct.hpp"
#include "structs/mesh_struct.hpp"
#include "utilities/image.hpp"
#include "utilities/shapes.hpp"
#include "utilities/utils.hpp"
#include "window.hpp"



Gloom::Shader *shader;
Camera *camera;
Keyboard *keyboard;
SkyboxHandler *skybox;
FrameBufferHandler *framebuffers;

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
    if (key == GLFW_KEY_L && action == GLFW_PRESS) skybox->swapCubemap();
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) camera->decreaseSensitivity();
    if (key == GLFW_KEY_E && action == GLFW_PRESS) camera->increaseSensitivity();
    if (key == GLFW_KEY_R && action == GLFW_PRESS) camera->resetSensitivity();
    if (key == GLFW_KEY_P && action == GLFW_PRESS) rotationSpeed = rotationSpeed == 15 ? 0 : 15;
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) bustAppearanceIndex++;
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) bustAppearanceIndex--;
    if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
    {
        bustAppearanceIndex = bustAppearanceIndex % BUST_APPEARANCES.size();
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

    skybox       = new SkyboxHandler();
    framebuffers = new FrameBufferHandler();
    keyboard     = new Keyboard();
    camera       = new Camera(glm::vec3(0, 10, 50), -10.0);

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
void updateState(GLFWwindow *window, double deltaTime)
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



/**
 * @brief Updates all transformations for node and recursively for all its children
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
    node->N   = glm::mat3(glm::transpose(glm::inverse(node->M)));
    node->MVP = VP * node->M;

    // Update children
    for (SceneNode *child : node->children) updateNodeTransformations(child, node->M, VP);
}



/** Recursively Render node and its children */
void renderNode(SceneNode *node)
{
    bool isRenderable = node->vao.ID != -1;
    if (isRenderable)
    {
        // Pass transformation matrices to vertex shader
        shader->setUniform(UNIFORMS::M, node->M);
        shader->setUniform(UNIFORMS::MVP, node->MVP);
        shader->setUniform(UNIFORMS::N, node->N);

        // Pass material information to fragment shader
        shader->setUniform("appearance.color", node->appearance.color);
        shader->setUniform("appearance.opacity", node->appearance.opacity);
        shader->setUniform("appearance.roughness", node->appearance.roughness);
        shader->setUniform("appearance.reflectivity", node->appearance.reflectivity);
        shader->setUniform("appearance.refraction_index", node->appearance.refractionIndex);

        if (!node->appearance.hasTexture)
        {
            shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::GEOMETRY_SHAPE);
        }
        else
        {
            shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::GEOMETRY_TEXTURED);
            shader->setUniform("appearance.use_texture_map", node->appearance.useTextureMap);

            glBindTextureUnit(BINDINGS::texture_map, node->appearance.texture.colorID);
            glBindTextureUnit(BINDINGS::normal_map, node->appearance.texture.normalID);
            glBindTextureUnit(BINDINGS::roughness_map, node->appearance.texture.roughnessID);
        }
        // Bind this nodes vertices and draw
        glBindVertexArray(node->vao.ID);
        glDrawElements(GL_TRIANGLES, node->vao.indexCount, GL_UNSIGNED_INT, nullptr);
    }
    // Render children
    for (SceneNode *child : node->children) renderNode(child);
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
    framebuffers->activateRefractionBuffer();

    // Calculate ViewProjection Matrix
    glm::mat4 projection = UTILS::getPerspectiveMatrix(90);
    glm::mat4 view       = camera->getViewMatrix();

    // Update Transformation for SceneGraph
    updateNodeTransformations(root, glm::mat4(1), projection * view);

    shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::REFRACTION);
    shader->setUniform("camera.position", camera->getPosition());

    glCullFace(GL_FRONT);
    renderNode(root);
    glCullFace(GL_BACK);
    glBindTextureUnit(11, framebuffers->refractionFramebuffer.textureID);
}



/**
 * For reflections the scene is rendered 6 times (at full scale) from the center of an object,
 * we then create a cubemap from these 6 textures that can bused to sample the reflections from
 * in the final render pass.
 */
void renderReflectionStep(GLFWwindow *window)
{
    std::vector<glm::vec3> viewDirections = {
        glm::vec3(1, 0, 0),  // right
        glm::vec3(-1, 0, 0), // left
        glm::vec3(0, 1, 0),  // top
        glm::vec3(0, -1, 0), // bottom
        glm::vec3(0, 0, 1),  // front
        glm::vec3(0, 0, -1)  // back
    };
    // Use up vectors to rotate faces correctly
    std::vector<glm::vec3> upDirections = {
        glm::vec3(0, -1, 0), // right
        glm::vec3(0, -1, 0), // left
        glm::vec3(0, 0, 1),  // top
        glm::vec3(0, 0, -1), // bottom
        glm::vec3(0, -1, 0), // front
        glm::vec3(0, -1, 0)  // back
    };

    framebuffers->activateCubemapBuffer();
    glm::mat4 projection = UTILS::getPerspectiveMatrix(90.0);

    for (unsigned int side = 0; side < 6; side++)
    {
        framebuffers->selectCubemapTarget(side);

        glm::mat4 view       = UTILS::getViewMatrix(root->position, viewDirections[side], upDirections[side]);
        if(side != 0 && side != 1){
            projection = UTILS::getPerspectiveMatrix(59.0);
        }

        skybox->updateVP(view, projection);
        updateNodeTransformations(root, glm::mat4(1), projection * view);

        shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::SKYBOX);
        shader->setUniform(UNIFORMS::MVP, skybox->VP);
        // Render skybox
        glDepthMask(GL_FALSE);
        glBindVertexArray(skybox->vaoID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTextureID());
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);


        shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::REFLECTION);
        shader->setUniform("camera.position", root->position);
        shader->setUniform("sun.color", skybox->getSunColor());
        shader->setUniform("sun.direction", skybox->getSunDirection());

        renderNode(root);
    }
    glBindTextureUnit(BINDINGS::reflection_cubemap, framebuffers->cubemapFrameBuffer.textureID);
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

    skybox->updateVP(view, projection);
    updateNodeTransformations(root, glm::mat4(1), projection * view);

    // Activate correct framebuffer
    framebuffers->activateScreenBuffer();
    shader->setUniform(UNIFORMS::PASS, UNIFORM_FLAGS::RENDER);

    // Render skybox
    shader->setUniform(UNIFORMS::TYPE, UNIFORM_FLAGS::SKYBOX);
    shader->setUniform(UNIFORMS::MVP, skybox->VP);
    glDepthMask(GL_FALSE);
    glBindVertexArray(skybox->vaoID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTextureID());
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);

    // Render the rest of the scene
    shader->setUniform("camera.position", camera->getPosition());
    shader->setUniform("sun.color", skybox->getSunColor());
    shader->setUniform("sun.direction", skybox->getSunDirection());
    renderNode(root);
}