#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double x, double y);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);
//void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

struct keyControllor
{
    float light_intensity = 0.6f;
    float position_x = 0.0f;
    float position_z = 0.0f;
    float rotate = 0.0f;
};

struct mouseControllor
{
    int LEFT_BUTTON = 0;
    int click_time;
    double lastX = 0, lastY = 0;
    double yaw = 0.0;
    double pitch = 0.0;
};

keyControllor keyCtl;
mouseControllor mouseCtl;
unsigned int skyboxVAO;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void createSkybox()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader shader("VertexShaderCode.glsl", "FragmentShaderCode.glsl");
    Shader skyboxShader("skyboxVertex.glsl", "skyboxFragment.glsl");

    // load models
    // -----------
    Model spacecraft("object/spacecraft/spacecraft.obj");
    Model planet("object/planet/planet.obj");
    Model vehicle("object/vehicles/craft.obj");

    // create skybox
    createSkybox();
    vector<std::string> faces
    {
        "skybox/right.bmp",
        "skybox/left.bmp",
        "skybox/top.bmp",
        "skybox/bottom.bmp",
        "skybox/front.bmp",
        "skybox/back.bmp"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 10);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        //processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        shader.use();

        // view matrix
        glm::vec3 eyePosition(0.0f + mouseCtl.yaw, 10.0f + mouseCtl.pitch, 20.0f);
        shader.setVec3("eyePositionWorld", eyePosition);

        glm::mat4 viewMatrix = glm::lookAt(eyePosition,
            glm::vec3(0.0f, 3.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("view", viewMatrix);

        // projection matrix
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
        shader.setMat4("projection", projectionMatrix);

        // point light
        glm::vec3 position(-2.0f, 10.0f, 20.0f);
        shader.setVec3("pointPosition", position);
        glm::vec3 pointColor(1.0f, 1.0f, 1.0f);
        shader.setVec3("pointColor", pointColor);
        shader.setFloat("point.constant", 1.0);
        shader.setFloat("point.linear", 0.01);
        shader.setFloat("point.quadratic", 0.0);


        // render the loaded model
        //spacecraft
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -200.0f, 0.0f)); // translate it down so it's at the center of the scene
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f));	// it's a bit too big for our scene, so scale it down
        glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = scaleMatrix * translateMatrix;
        shader.setMat4("model", model);
        spacecraft.Draw(shader);

        //planet
        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f));
        model = translateMatrix * rotateMatrix;
        shader.setMat4("model", model);
        planet.Draw(shader);

        // 3 vehicles
        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 7.0f, -40.0f));
        scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
        model = translateMatrix * scaleMatrix;
        shader.setMat4("model", model);
        vehicle.Draw(shader);

        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, -10.0f, -35.0f));
        model = translateMatrix * scaleMatrix;
        shader.setMat4("model", model);
        vehicle.Draw(shader);

        translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -40.0f));
        model = translateMatrix * scaleMatrix;
        shader.setMat4("model", model);
        vehicle.Draw(shader);

        // skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        viewMatrix = glm::mat4(glm::mat3(viewMatrix)); // remove translation from the view matrix
        skyboxShader.setMat4("view", viewMatrix);
        skyboxShader.setMat4("projection", projectionMatrix);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glDepthFunc(GL_LESS);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Sets the mouse-button callback for the current window.	
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        mouseCtl.LEFT_BUTTON = 1;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouseCtl.LEFT_BUTTON = 0;
        mouseCtl.lastX = 0; //reset
    }
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    //// Sets the cursor position callback for the current window
    if (mouseCtl.LEFT_BUTTON)
    {
        // check whether requiring initialization
        if (mouseCtl.lastX == 0)
        {
            mouseCtl.lastX = x;
            mouseCtl.lastY = y;
            return;
        }

        double xoffset = x - mouseCtl.lastX;
        double yoffset = mouseCtl.lastY - y;
        mouseCtl.lastX = x;
        mouseCtl.lastY = y;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        mouseCtl.yaw += xoffset;
        mouseCtl.pitch += yoffset;

        if (mouseCtl.pitch > 10.0f)
            mouseCtl.pitch = 10.0f;
        if (mouseCtl.pitch < -6.0f)
            mouseCtl.pitch = -6.0f;
        if (mouseCtl.yaw > 15.0f)
            mouseCtl.yaw = 15.0f;
        if (mouseCtl.yaw < -15.0f)
            mouseCtl.yaw = -15.0f;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //Sets the scoll callback for the current window.
    //zoom
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}