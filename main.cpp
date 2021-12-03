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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(vector<std::string> faces);
int RandomNum(int maxiNum);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1080;
const unsigned int SCR_HEIGHT = 720;

struct keyControllor
{
    float light_intensity = 0.6f;
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;
    float position_x = 0.0f;
    float position_z = 0.0f;
    float last_position_x = 0.0f;
    float last_position_z = 0.0f;
    float rotate = 0.0f;
    int normal = 0;
};

struct mouseControllor
{
    double lastX = -1.0f;
    double yaw = 0.0f;
};

keyControllor keyCtl;
mouseControllor mouseCtl;
unsigned int skyboxVAO;
float fov = 60.0f;
float last_position_x = 0.0f;
float last_position_z = 0.0f;
const int amount = 200;
glm::mat4 modelMatrices[amount];

// num of gold collected
int numOfTreasureCollected = 0;
// whether is collected
int treasureColl[8] = { 1, 1, 1, 1, 1, 2, 2, 2}; // 1 for gold and 2 for diamond

// random position of vehicles
int move1 = RandomNum(12);
int move2 = RandomNum(15);
int move3 = RandomNum(12);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int flag = -1;

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

void CreateRead_ModelM() {

    // initial random seed
    srand(glfwGetTime());
    GLfloat radius = 6.0f;
    GLfloat offset = 0.4f;
    GLfloat displacement;
    for (GLuint i = 0; i < amount; i++) {
        glm::mat4 model;
        // 1. Translation: Randomly displace along circle with radius in range [-offset,offset]
        GLfloat angle = (GLfloat)i / (GLfloat)amount * 360.0f;
        // x
        displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
        GLfloat x = sin(angle) * radius + displacement;
        // y
        displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
        GLfloat y = displacement * 0.4f + 1;
        // z
        displacement = (rand() % (GLint)(2 * offset * 200)) / 100.0f - offset;
        GLfloat z = cos(angle) * radius + displacement;
        model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        // 2. Scale: Scale between 0.05 and 0.25f
        GLfloat scale = (rand() % 10) / 100.0f + 0.05;
        model = glm::scale(model, glm::vec3(scale));
        // 3. Rotation: add random rotation around a ( semi )randomly picked rotation axis vector
        GLfloat rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
        // 4. Now add to list of matrices
        modelMatrices[i] = model;
    }
}

int RandomNum(int maxiNum) {
    int rangX = maxiNum * 2;
    return rand() % rangX - maxiNum;
}

bool CollisionDetection(glm::vec4 vectorA, glm::vec4 vectorB, int dis) {
    if (glm::distance(vectorA, vectorB) <= dis)
        return true;
    else
        return false;
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
    glfwSetKeyCallback(window, key_callback);

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
    Model spacecraft1("object/spacecraft/spacecraft.obj");
    Model spacecraft2("object/spacecraft/spacecraft2.obj");
    Model planet("object/planet/planet.obj");
    Model vehicle1("object/vehicles/craft.obj");
    Model vehicle2("object/vehicles/craft2.obj");
    Model rock("object/rock/rock.obj");
    Model gold("object/rock/rock2.obj");
    Model diamond("object/diamond/diamond.obj");

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

    CreateRead_ModelM();
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float timer = deltaTime * 150;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        shader.use();

        // view matrix
        float angle = -mouseCtl.yaw;
        float deltax = keyCtl.position_x - keyCtl.last_position_x;
        float deltaz = keyCtl.position_z - keyCtl.last_position_z;
        float new_position_x = last_position_x + deltax * glm::cos(angle) + deltaz * glm::sin(angle);
        float new_position_z = last_position_z + deltaz * glm::cos(angle) - deltax * glm::sin(angle);
        glm::vec3 eyePosition(new_position_x + 1.19f * glm::sin(angle),
                              1.2f, 
                              new_position_z + 1.19f * glm::cos(angle));
        shader.setVec3("eyePositionWorld", eyePosition);

        glm::mat4 viewMatrix = glm::lookAt(eyePosition,
          glm::vec3(new_position_x,
                    0.8f, 
                    new_position_z),
          glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("view", viewMatrix);

        // projection matrix
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
        shader.setMat4("projection", projectionMatrix);

        // point light
        glm::vec3 position(0.0f, 5.0f, -55.0f);
        shader.setVec3("pointPosition", position);
        glm::vec3 pointColor(1.0f, 1.0f, 1.0f);
        shader.setVec3("pointColor", pointColor);
        shader.setFloat("point.constant", 1.0f);
        shader.setFloat("point.linear", 0.04f);
        shader.setFloat("point.quadratic", 0.01f);

        // parallel light source
            glm::vec3 dirDirection(0.0f, -1.0f, 0.0f);
            shader.setVec3("dirDirection", dirDirection);
            glm::vec3 dirColor(1.0f, 1.0f, 1.0f);
            shader.setVec3("dirColor", dirColor);
            float intensity = 0.5f;
            shader.setFloat("intensity", intensity);

        // render the loaded model
        //spacecraft       
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 translateMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f)); // translate it up
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));	// scale it down
        glm::mat4 rotateMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));       
        glm::mat4 rotateMatrix2 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f)); // change direction by mouse
        
        // change position by keyboard
        glm::mat4 translateMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(last_position_x, 0.0f, last_position_z));
        
        glm::mat4 translateMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(deltax * glm::cos(angle) + deltaz * glm::sin(angle),
            0.0f,
            deltaz * glm::cos(angle) - deltax * glm::sin(angle))); 
        last_position_x = new_position_x;
        last_position_z = new_position_z;
        glm::mat4 craftmodel = translateMatrix3 * translateMatrix2 * rotateMatrix2 * translateMatrix1 * scaleMatrix * rotateMatrix1;
        shader.setMat4("model", craftmodel);
        shader.setBool("normal_flag", 0);
        if (numOfTreasureCollected == 8) {
            spacecraft2.Draw(shader);
        }
        else {
            spacecraft1.Draw(shader);
        }

        //planet
        translateMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f));
        rotateMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
        scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f));
        model =  translateMatrix1 * scaleMatrix * rotateMatrix2 * rotateMatrix1;
        shader.setMat4("model", model);
        if(keyCtl.normal % 2 == 1)
        shader.setBool("normal_flag", 1);
        planet.Draw(shader);
        shader.setBool("normal_flag", 0);

        // 3 vehicles
        int current = (int) 2 * currentFrame;
        if (current % 2 == 1)
        {
            int flag2 = current / 2;
            if ( flag2 > flag)
            {
                move1 = RandomNum(10);
                move2 = RandomNum(10);
                move3 = RandomNum(10);
                flag = flag2;    
            }
        }
        translateMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(move1, 0.0f, -12.0f));
        scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
        rotateMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(currentFrame * 2), glm::vec3(0.0f, 1.0f, 0.0f));
        model = translateMatrix1 * scaleMatrix * rotateMatrix2;
        shader.setMat4("model", model);
        
        if (!CollisionDetection(model * glm::vec4(0, 0, 0, 1), craftmodel * glm::vec4(0, 0, 0, 1), 5)) {
            vehicle1.Draw(shader);
        }
        else {
            vehicle2.Draw(shader);
        }

        translateMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(move2, 0.0f, -25.0f));
        model = translateMatrix1 * scaleMatrix * rotateMatrix2;
        shader.setMat4("model", model);
        if (!CollisionDetection(model * glm::vec4(0, 0, 0, 1), craftmodel * glm::vec4(0, 0, 0, 1), 5)) {
            vehicle1.Draw(shader);
        }
        else {
            vehicle2.Draw(shader);
        }

        translateMatrix1 = glm::translate(glm::mat4(1.0f), glm::vec3(move3, 0.0f, -40.0f));
        model = translateMatrix1 * scaleMatrix * rotateMatrix2;
        shader.setMat4("model", model);
        if (!CollisionDetection(model * glm::vec4(0, 0, 0, 1), craftmodel * glm::vec4(0, 0, 0, 1), 5)) {
            vehicle1.Draw(shader);
        }
        else {
            vehicle2.Draw(shader);
        }

        // rock
        glm::mat4 rockOrbitIni = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -50.0f));
        glm::mat4 rockOrbit_M = glm::rotate(rockOrbitIni, glm::radians(currentFrame * 2), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rockModelMat_temp;

        // calculate the distance between gold and aircraft
        for (int i = 0; i < 8; i++) {
            if (treasureColl[i] > 0 && CollisionDetection(rockOrbit_M * modelMatrices[i * 25] * glm::vec4(0, 0, 0, 1), craftmodel * glm::vec4(0, 0, 0, 1), 2))
            {
                treasureColl[i] = 0;
                numOfTreasureCollected++;
            }
        }
        for (GLuint i = 0; i < amount; i++) {
            rockModelMat_temp = modelMatrices[i];
            rockModelMat_temp = rockOrbit_M * rockModelMat_temp;

            shader.setMat4("model", rockModelMat_temp);
            if (i % 25 == 0) {
                if (treasureColl[i / 25] == 1)
                    gold.Draw(shader);
                if (treasureColl[i / 25] == 2)
                    diamond.Draw(shader);
            }
            else {
                rock.Draw(shader);
            }
        }

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


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

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
    //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    //{
    //    mouseCtl.LEFT_BUTTON = 1;
    //}
    //if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    //{
    //    mouseCtl.LEFT_BUTTON = 0;
    //    mouseCtl.lastX = 0; //reset
    //}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    //// Sets the cursor position callback for the current window
    if(mouseCtl.lastX == -1)
        mouseCtl.lastX = x;
    double xoffset = x - mouseCtl.lastX;
    //double yoffset = mouseCtl.lastY - y;
    mouseCtl.lastX = x;
    //mouseCtl.lastY = y;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    //yoffset *= sensitivity;

    mouseCtl.yaw += xoffset;
    //mouseCtl.pitch += yoffset; 
}

//  glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //Sets the scoll callback for the current window.
    //zoom
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 70.0f)
        fov = 70.0f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Sets the Keyboard callback for the current window.
    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        keyCtl.normal += 1;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        keyCtl.left = 1;
    }
    if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE)
    {
        keyCtl.left = 0;
        keyCtl.last_position_x = keyCtl.position_x;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        keyCtl.right = 1;
    }
    if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)
    {
        keyCtl.right = 0;
        keyCtl.last_position_x = keyCtl.position_x;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        keyCtl.up = 1;
    }
    if (key == GLFW_KEY_UP && action == GLFW_RELEASE)
    {
        keyCtl.up = 0;
        keyCtl.last_position_z = keyCtl.position_z;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        keyCtl.down = 1;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE)
    {
        keyCtl.down = 0;
        keyCtl.last_position_z = keyCtl.position_z;
    }
    if (keyCtl.left == 1)
    {
        keyCtl.last_position_x = keyCtl.position_x;
        keyCtl.position_x -= 0.04f;
    }
    if (keyCtl.right == 1)
    {
        keyCtl.last_position_x = keyCtl.position_x;
        keyCtl.position_x += 0.04f;
    }
    if (keyCtl.up == 1)
    {
        keyCtl.last_position_z = keyCtl.position_z;
        keyCtl.position_z -= 0.04f;
    }
    if (keyCtl.down == 1)
    {
        keyCtl.last_position_z = keyCtl.position_z;
        keyCtl.position_z += 0.04f;
    }
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