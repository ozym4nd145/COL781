#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/string_cast.hpp>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <deque>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

#include "wall.h"
#include "skybox.h"
#include "beizer.h"
#include "light.h"
#include "fbo.h"
#include "quad.h"
#include "terrain.h"
#include "water.h"


#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const float PIE = 3.14159f;

float calculateMixRatio(float curTime,float start, float duration) {
    float ratio = min(max((curTime-start)/duration,0.0f),1.0f);
    // cout<<ratio<<endl;
    ratio *= 5.0f;
    // cout<<ratio<<endl;
    ratio -= 2.5f;
    // cout<<ratio<<endl;
    ratio = 1.0f/(1.0f+exp(-ratio));
    // cout<<ratio<<endl;
    ratio -= 0.5f;
    // cout<<ratio<<endl;
    ratio *= 1.2f;
    // cout<<ratio<<endl;
    ratio += 0.5f;
    // cout<<ratio<<endl;
    ratio = min(max(ratio,0.0f),1.0f);
    // cout<<ratio<<endl;
    return ratio;
}

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Genesis - The Wrath of Khan", NULL, NULL);
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
    Shader modelShader("../resources/shaders/model.vs", "../resources/shaders/model.fs");
    Shader skyboxShader("../resources/shaders/skybox.vs", "../resources/shaders/skybox.fs");
    Shader screenShader("../resources/shaders/fbo.vs","../resources/shaders/fbo.fs");
    Shader terrainShader("../resources/shaders/terrain.vs", "../resources/shaders/terrain.fs");

    Wall wall_of_fire(1e5,1.75f,glm::vec3(0.0f),1.0f);
    Model moon("../models/my_moon/moon.obj");

    vector<std::string> spaceFaces = {
        "../resources/textures/night/nightRight.png",
        "../resources/textures/night/nightLeft.png",
        "../resources/textures/night/nightTop.png",
        "../resources/textures/night/nightBottom.png",
        "../resources/textures/night/nightFront.png",
        "../resources/textures/night/nightBack.png"
    };
    SkyBox spaceSkyBox(spaceFaces);

    vector<std::string> dayFaces = {
        "../resources/textures/sor_sea/left.JPG",
        "../resources/textures/sor_sea/right.JPG",
        "../resources/textures/sor_sea/top.JPG",
        "../resources/textures/sor_sea/bottom.JPG",
        "../resources/textures/sor_sea/front.JPG",
        "../resources/textures/sor_sea/back.JPG"
    };
    SkyBox daySkyBox(dayFaces);

    LightScene lightScene(glm::vec3(0.1f,0.1f,0.1f),{
        PointLight{10000.0f*glm::vec3(sin(PIE/6.0f)*cos(PIE/6.0f),sin(PIE/6.0f),cos(PIE/6.0f)*cos(PIE/6.0f)),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,0.0f,0.0f)}
    });

    int terrainSize = 500;
    int waterSize = 4000;
    int terrainCount = 1000;
    int terrainHeight = 100;
    Terrain ground(-terrainSize/2,-terrainSize/2,terrainSize,terrainCount,terrainHeight,{
                                        "../resources/textures/grass.png",
                                        "../resources/textures/water.jpg",
                                        "../resources/textures/rock.jpg",
                                        "../resources/textures/snow.jpg",
                                        },"../resources/textures/heightmap.png");
    Water water(glm::vec3(-waterSize/2,ground.sea_height,-waterSize/2),waterSize,5,{"../resources/textures/water.jpg"});


    vector<glm::vec3> camera_points = {
        glm::vec3(0.0f,0.0f,10.0f),
        glm::vec3(0.0f,2.75f,5.75f),
        glm::vec3(0.0f,5.1f,0.0f),
        glm::vec3(0.0f,0.1f,-5.0f),
        glm::vec3(0.0f,-1.8f,0.0f),
    };
    vector<glm::vec3> camera_yaw_pitch = {
        glm::vec3(-90.0f,0.0f,0.0f),
        glm::vec3(-110.0f,-10.0f,0.0f),
        glm::vec3(-140.0f,-30.0f,0.0f),
        glm::vec3(-130.0f,-60.0f,0.0f),
        glm::vec3(-110.0f,-120.0f,0.0f),
        glm::vec3(-90.0f,-180.0f,0.0f),
        glm::vec3(-90.0f,-160.0f,0.0f),
        glm::vec3(-90.0f,-140.0f,0.0f),
    };

    Beizer bcurve_loc(camera_points);
    Beizer bcurve_view(camera_yaw_pitch);
    float movement_time = 17.0f;
    float transition_time = 5.0f;

    glm::vec3 spaceColor{0.1f,0.1f,0.1f};
    glm::vec3 skyColor{0.53, 0.81, 0.98};
    // glm::vec3 skyColor{0.27,0.42,0.58};


    deque<FBO*> motionBlurFBOs;
    FBO fireFBO(SCR_WIDTH,SCR_HEIGHT);
    fireFBO.mount();
    glClearColor(spaceColor[0],spaceColor[1],spaceColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    FBO planetFBO(SCR_WIDTH,SCR_HEIGHT);
    planetFBO.mount();
    glClearColor(skyColor[0],skyColor[1],skyColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const int NUM_ACCUM=10;
    for(int i=0;i<NUM_ACCUM;i++) {
        FBO* fbo = new FBO(SCR_WIDTH,SCR_HEIGHT);
        fbo->mount();
        glClearColor(spaceColor[0],spaceColor[1],spaceColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        motionBlurFBOs.push_back(fbo);
    }

    Quad screen;

    // render loop
    // -----------
    float initFrame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        FBO* curFBO = motionBlurFBOs.front();
        motionBlurFBOs.pop_front();
        motionBlurFBOs.push_back(curFBO);

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

        float mixRatio = calculateMixRatio(timePassed,movement_time-transition_time-0.5f,transition_time);
        
        // render
        // ------
        if(mixRatio < 1.0f) {
            fireFBO.mount();
            glClearColor(spaceColor[0],spaceColor[1],spaceColor[2], 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            float bcurve_t = min((timePassed)/movement_time,0.99f);
            glm::vec3 camera_pres_pos = bcurve_loc.get_pt(bcurve_t);
            glm::vec3 camera_pres_dir = bcurve_view.get_pt(bcurve_t);
            camera.setPosition(camera_pres_pos);
            camera.setYawPitch(camera_pres_dir.x,camera_pres_dir.y);

            particleShader.use();

            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::vec3 cameraPos = camera.Position;

            glm::mat4 view = camera.GetViewMatrix();
            particleShader.setMat4("projection", projection);
            particleShader.setMat4("view", view);

            if(timePassed > 0.2f){
                wall_of_fire.Update(deltaTime,timePassed-0.2f,cameraPos);
                wall_of_fire.Draw(particleShader);
            }


            modelShader.use();

            // view/projection transformations
            modelShader.setMat4("projection", projection);
            modelShader.setMat4("view", view);

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            modelShader.setMat4("model", model);
            lightScene.configureLights(modelShader);
            modelShader.setFloat("shineDamper",1.0f);
            modelShader.setFloat("reflectivity",0.0f);
            modelShader.setVec3("viewPos",cameraPos);
            modelShader.setFloat("heightScale",0.01);

            moon.Draw(modelShader);

            skyboxShader.use();
            view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);

            spaceSkyBox.Draw();
        }
        
        if(mixRatio > 0.0f) {
            if(mixRatio<1.0f) {
                camera.setPosition(glm::vec3(0.0f,ground.mountain_limit,0.0f));
                camera.setYawPitch(-90.0f,0.0f);
            }

            planetFBO.mount();
            glClearColor(skyColor[0],skyColor[1],skyColor[2], 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
            glm::vec3 cameraPos = camera.Position;
            glm::mat4 view = camera.GetViewMatrix();

            terrainShader.use();
            lightScene.configureLights(terrainShader);

            terrainShader.setVec3("skyColor",skyColor);
            terrainShader.setFloat("shineDamper",1.0f);
            terrainShader.setFloat("reflectivity",0.0f);
            terrainShader.setVec3("viewPos",cameraPos);
            terrainShader.setMat4("projection",projection);
            terrainShader.setMat4("view",view);
            ground.Draw(terrainShader);

            terrainShader.setFloat("shineDamper",100.0f);
            terrainShader.setFloat("reflectivity",1.0f);
            water.Draw(terrainShader);

            // skyboxShader.use();
            // view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
            // skyboxShader.setMat4("view", view);
            // skyboxShader.setMat4("projection", projection);
            // daySkyBox.Draw();
        }

        curFBO->mount();
        glClearColor(0.0,0.0,0.0, 1.0f);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        
        screenShader.use();
        screenShader.setFloat("opacity",1.0f);
        screen.Draw(screenShader,{fireFBO.getTexture().id});
        screenShader.setFloat("opacity",mixRatio);
        screen.Draw(screenShader,{planetFBO.getTexture().id});


        curFBO->unmount();
        glClearColor(0.0,0.0,0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        

        screenShader.use();
        screenShader.setFloat("opacity",0.8f);
        glDisable(GL_DEPTH_TEST);
        
        for(auto fbo: motionBlurFBOs) {
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
