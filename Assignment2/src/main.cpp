#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

#include <iostream>
#include "AnimatedModel.h"

#include "beizer.h"
#include "ball.h"
#include "pin.h"
#include "track.h"
#include "gutter.h"
#include "engine.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(21.0f, 10.0f, 35.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main(int argc, char** argv)
{

    // Necessary to position the camera at a position from where, we can view the complete scene.
    camera.setYawPitch(-135.0f,-5.0f);

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

    Shader ourShader("../resources/shaders/model.vs", "../resources/shaders/model.fs");
    Shader animatedModelShader("../resources/shaders/animated.vs", "../resources/shaders/animated.fs");

    // glLineWidth(LINE_WIDTH);

    std::vector<glm::vec3> ball_bcurve_track_points = {glm::vec3(1.0f,1.0f,23.0f), glm::vec3(0.0f,0.0f,20.0f), glm::vec3(-2.0f,0.0f,5.0f), glm::vec3(-2.0f,0.0f,-8.0f), glm::vec3(0.0f,0.0f,-18.0f)};
    std::vector<glm::vec3> ball_bcurve_hand_points = {glm::vec3(1.0f,1.0f,30.0f), glm::vec3(1.0f,1.0f,23.0f)};


    Beizer *bcurve_hand = new Beizer(ball_bcurve_hand_points);
    Beizer *bcurve_track = new Beizer(ball_bcurve_track_points);
    const float speed_hand_ball  = 1.0f;
    const float speed_track_ball  = 8.0f;
    const float time_to_fall  = 1.0f;
    const float track_ball_start = 6.5f;
    const bool camera_follows = true;
    glm::vec3 camera_offset = glm::vec3(0.0f,5.0f,12.0f);
    
    Ball ballHand(std::string("../models/obj/ball.obj"),&ourShader,bcurve_hand,speed_hand_ball);
    Ball ballTrack(std::string("../models/obj/ball.obj"),&ourShader,bcurve_track,speed_track_ball);
    Pin pin(std::string("../models/obj/pin.obj"),&ourShader,time_to_fall);
    Track track(std::string("../models/obj/track.obj"),&ourShader);
    Gutter gutter(std::string("../models/obj/gutter.obj"),&ourShader);

    
    Engine engine(&ballTrack,&ballHand,&pin,&track,&gutter,track_ball_start);

    anim::AnimatedModel human("../resources/bowler/bowler.dae");

    // render loop
    // -----------
    float initFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        if(camera_follows && timePassed>=track_ball_start){
            camera.resetYawPitch();
            glm::vec3 ball_pos = ballTrack.get_center(timePassed-track_ball_start);
            camera.setPosition(ball_pos + camera_offset);
        }
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        engine.draw_at_time(timePassed);

        animatedModelShader.use();
        animatedModelShader.setMat4("projection", projection);
        animatedModelShader.setMat4("view", view);
        human.update(timePassed);
        human.Draw(animatedModelShader);

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
