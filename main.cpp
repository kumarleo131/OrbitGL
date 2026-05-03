#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "stb_image.h"
#include "camera.h"

// keep track of time
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float charWidth = 6;   // horizontal size on screen
float charHeight = 16; // vertical size on screen

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// For processing input method
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int LoadTexture(const char* path);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);




int main()
{
    glfwInit();
    // Tell GLFW we want to use version 3.3 of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); ONLY NEEDED OR MAC (LOSER)

    // Create the window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OrbitGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // Tell OpenGL Sise of the rendering window(coordinate system)
    // The first two parameters of glViewport set the location of the lower left corner of the window.
    // The third and fourth parameter set the width and height of the rendering window in pixels, which we set equal to GLFW's window size.
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT); //Height was 600

    // We do have to tell GLFW we want to call this function on every window resize by registering it:
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);
    // build and compile shader program
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Shader lightCubeShader("shaders/cubever.glsl", "shaders/cubefrag.glsl");

    glm::vec3 coral(1.0f, 0.5f, 0.31f);

    glm::vec3 lightColor(0.33f, 0.42f, 0.18f);
    glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
    glm::vec3 result = lightColor * toyColor; // = (0.33f, 0.21f, 0.06f);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



    // Because we want to render a single triangle we want to specify a total of three vertices with each vertex having a 3D position.
    // We define them in normalized device coordinates (the visible region of OpenGL) in a float array:
   float vertices[] = {
    // positions          // normals           // texcoords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f, 0.0f,   1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,    0.0f, 1.0f
};
    float lampVertices[] = {
        // positions         // normals       // texcoords
        // Front face
        -0.5f, -0.5f,  0.5f,  0, 0, 1,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0, 0, 1,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0, 0, 1,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0, 0, 1,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0, 0, 1,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0, 0, 1,   0.0f, 0.0f,

        // Back face
        -0.5f, -0.5f, -0.5f,  0, 0, -1,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0, 0, -1,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0, 0, -1,   1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0, 0, -1,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0, 0, -1,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0, 0, -1,   0.0f, 0.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, -1, 0, 0,   1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1, 0, 0,   1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1, 0, 0,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1, 0, 0,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1, 0, 0,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1, 0, 0,   1.0f, 0.0f,

        // Right face
         0.5f,  0.5f,  0.5f, 1, 0, 0,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1, 0, 0,    1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1, 0, 0,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1, 0, 0,    0.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 1, 0, 0,    0.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 1, 0, 0,    1.0f, 0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f, 0, 1, 0,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0, 1, 0,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0, 1, 0,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0, 1, 0,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0, 1, 0,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0, 1, 0,    0.0f, 1.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0, -1, 0,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0, -1, 0,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0, -1, 0,   1.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0, -1, 0,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0, -1, 0,   0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0, -1, 0,   0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


    // unsigned int indices[] = {
    //     0,1,2, 2,3,0,        // Front
    //     4,5,6, 6,7,4,        // Back
    //     8,9,10, 10,11,8,     // Left
    //     12,13,14, 14,15,12,  // Right
    //     16,17,18, 18,19,16,  // Bottom
    //     20,21,22, 22,23,20   // Top
    // };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);



    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO, lampVBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &lampVBO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);
    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  //  glBindVertexArray(0);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);




    glBindVertexArray(cubeVAO);
    stbi_set_flip_vertically_on_load(true);
    unsigned int texture = LoadTexture("textures/dirt.jpg");
    unsigned int lamp = LoadTexture("textures/lamp.jpg");
    shader.use();
    shader.setInt("ourTexture", 0);
    shader.setInt("lightTexture", 1);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texcoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //
    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)800/ (float)600, 0.1f, 100.0f);
    // shader.setMat4("projection", projection);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup orthographic projection for top-left origin
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCR_WIDTH, SCR_HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // Render loop (every iteration of the render loop is referred to as a frame)
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        float brightness = (sin(glfwGetTime() * 0.2f) + 1.0f) / 4.0f;

        glClearColor(0.2f * brightness,
                     0.4f * brightness,
                     0.8f * brightness,
                     1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float radius = 25.0f;
        float speed = 0.2f;
        float lightX = 12.5f;
        float lightZ = cos(glfwGetTime() * speed) * radius;
        float lightY = 12.5f * sin(glfwGetTime() * speed);
        glm::vec3 lightPos = glm::vec3(lightX, lightY, lightZ);

        shader.use();
        shader.setVec3("lightPos", lightPos);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightColor", glm::vec3(1.0f));
        shader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        // Render cube
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture); // dirt.jpg
        shader.use();
        shader.setInt("ourTexture", 0);

        glBindVertexArray(cubeVAO);
        for (float i = 0; i < 25; i += 0.1f)
        {
            for (float j = 0; j < 25; j += 0.1f)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(i, 0.0f, j));
                model = glm::scale(model, glm::vec3(0.1f));
                shader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }



        // Render sun
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, lamp); // bind lamp texture
        lightCubeShader.use();

        // Send updated position to cube shader
        shader.use();
        shader.setVec3("lightPos", lightPos);

        // update lamp's position for rendering
        lightCubeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(1.0f));
        lightCubeShader.setMat4("model", model);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);

        // Draw lamp
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // swap buffers and poll IO events (keys pressed/released, mouse moved)
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    // As soon as we exit the render loop we would like to properly clean/delete all of GLFW's resources that were allocated.
    glfwTerminate();

    return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Input Control
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime); // if your Camera class supports UP/DOWN
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}


unsigned int LoadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

