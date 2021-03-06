#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>
#include <deque>

#include "watergun.h"
#include "dust.h"
#include "fbo.h"
#include "quad.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char** argv)
{

    // Necessary to position the camera at a position from where, we can view the complete scene.
    // camera.setYawPitch(-135.0f,-5.0f);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



    Shader particleShader("../resources/shaders/particle.vs", "../resources/shaders/particle.fs");
    Shader screenShader("../resources/shaders/fbo.vs","../resources/shaders/fbo.fs");

    WaterGun greenGun(10000,{20.0f,0.0f,-80.0f},{-1.0f,1.0f,0.0f},{0.54f, 0.38f, 0.15f,0.2f},15.0f,0.1,3.0f,5000);
    WaterGun blueGun(10000,{0.0f,0.0f,-80.0f},{0.0f,1.0f,0.0f},{0.0f,0.464f,0.742f,0.2f},20.0f,0.2,3.0f,5000);
    WaterGun purpleGun(10000,{-20.0f,0.0f,-80.0f},{1.0f,1.0f,0.0f},{0.464f,0.0f,0.742f,0.3f},15.0f,0.1,3.0f,5000);
    Dust redHoli(100000,{0.0f,0.0f,20.0f},{0.464f,0.1f,0.242f,0.3f},
                    {{0.0f,-10.0f,20.0f},{5.0f,10.0f,20.0f},{-25.0f,15.0f,20.0f},{-15.0f,5.0f,10.0f}},
                    {{0.0f,1.0f,0.0f},{-3.0f,1.0f,0.0f},{1.0,-0.5f,-1.0f},{0.2,1.5f,-1.0f}}
                    ,{2.0f,4.0f,6.0f,3.0f},
                    1.0f,20.0f,10000,30.0f);

    glm::vec3 backgroundColor{0.1f,0.1f,0.1f};

    deque<FBO*> fbos;
    const int NUM_ACCUM=10;
    for(int i=0;i<NUM_ACCUM;i++) {
        FBO* fbo = new FBO(SCR_WIDTH,SCR_HEIGHT);
        fbo->mount();
        glClearColor(backgroundColor[0],backgroundColor[1],backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fbos.push_back(fbo);
    }

    Quad screen;

    // render loop
    // -----------
    float initFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        FBO* curFBO = fbos.front();
        fbos.pop_front();
        fbos.push_back(curFBO);

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        float timePassed = currentFrame-initFrame;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // cout<<"Time passed: "<<timePassed<<endl;

        // input
        // -----
        processInput(window);

        // render
        // ------
        curFBO->mount();

        glClearColor(backgroundColor[0],backgroundColor[1],backgroundColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        particleShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 4000.0f);
        glm::vec3 cameraPos = camera.Position;

        glm::mat4 view = camera.GetViewMatrix();
        particleShader.setMat4("projection", projection);
        particleShader.setMat4("view", view);

        blueGun.Update(deltaTime,cameraPos);
        purpleGun.Update(deltaTime,cameraPos);
        greenGun.Update(deltaTime,cameraPos);
        redHoli.Update(deltaTime,timePassed,cameraPos);

        blueGun.Draw(particleShader);
        purpleGun.Draw(particleShader);
        greenGun.Draw(particleShader);
        redHoli.Draw(particleShader);

        // fbo unmount
        curFBO->unmount();

        glClearColor(0.0,0.0,0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        screenShader.use();
        screenShader.setFloat("opacity",0.8f);
        glDisable(GL_DEPTH_TEST);
        
        for(auto fbo: fbos) {
            screen.Draw(screenShader,{fbo->getTexture().id});
        }

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
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
