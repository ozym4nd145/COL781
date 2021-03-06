#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include "OGLModels.h"
#include "Camera.h"
#include "DS.h"
#include "Engine.h"
#include "Image.h"
#include "Models.h"
#include "defs.h"
#include "utils.h"


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const float LINE_WIDTH = 5.0f;
// camera
ogl::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// // lighting
// glm::vec3 lightPos(30.0f,20.0f,30.0f);
ogl::Lines* lineModel;
std::vector<std::vector<std::pair<Vector3f,Vector3f>>> lines(2);

int main(int argc, char** argv)
{
    const int width = 512;
    const int height = 512;

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <Input JSON file>" << endl;
        exit(-1);
    }

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
    glLineWidth(LINE_WIDTH);

    lines[0].push_back(make_pair(Vector3f(0,0,0),Vector3f(0,0,-1)));
    lines[0].push_back(make_pair(Vector3f(0,0,-1),Vector3f(5,0,-1)));
    lines[0].push_back(make_pair(Vector3f(5,0,-1),Vector3f(5,2,-1)));

    lines[1].push_back(make_pair(Vector3f(0,0,0),Vector3f(0,0,5)));
    lines[1].push_back(make_pair(Vector3f(0,0,5),Vector3f(5,0,5)));
    lines[1].push_back(make_pair(Vector3f(5,0,5),Vector3f(5,2,5)));


    string in_filename = string(argv[1]);
    State state = get_state(in_filename);

    Image img{width, height};
    RenderEngine render_man(*(state.cam), img, *(state.bg), state.models, state.lights,
                            Color(0.2, 0.2, 0.2));
    render_man.render();
    render_man.writeImage("./sphere.ppm");

    std::vector<pair<Point,Color>> lightVec;
    for(auto light: state.lights) {
        lightVec.push_back(make_pair(light->getCenter(),light->getIntensity()));
    }
    ogl::Lights lights(lightVec);
    ogl::CameraModel cameraModel(state.cam->getTransformation());
    
    int imgI = state.tracePoint.first;
    int imgJ = state.tracePoint.second;
    auto trace = render_man.getTrace(imgI,imgJ);
    std::cout<<"trace point: "<<imgI<<","<<imgJ<<" color: "<<trace.first<<" numLines: "<<(trace.second).size()<<std::endl;
    lineModel = new ogl::Lines(trace.second);

    // build and compile shaders
    // -------------------------
    ogl::Shader modelShader("../resources/shaders/model.vs", "../resources/shaders/model.fs");
    ogl::Shader simpleShader("../resources/shaders/simple.vs", "../resources/shaders/simple.fs");
    
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.7f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        modelShader.use();
        lights.configureLights(modelShader);
        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);

        cameraModel.Draw(modelShader);
        for(auto obm: state.oglModels) {
            obm->Draw(modelShader);
        }

        simpleShader.use();
        simpleShader.setMat4("projection", projection);
        simpleShader.setMat4("view", view);
        lights.DrawLights(simpleShader);
        if(lineModel)
            lineModel->Draw(simpleShader);

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
        camera.ProcessKeyboard(ogl::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(ogl::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(ogl::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(ogl::RIGHT, deltaTime);
    
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    //     lineModel->resetLines(lines[1]);
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    //     lineModel->resetLines(lines[0]);
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
