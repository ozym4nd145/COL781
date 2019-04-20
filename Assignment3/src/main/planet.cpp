#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/model.h>
#include <learnopengl/camera.h>

// #include "watergun.h"
// #include "dust.h"
#include "terrain.h"
#include "skybox.h"
#include "light.h"
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
Camera camera(glm::vec3(21.0f, 400.0f, 35.0f));
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
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );



    // Shader particleShader("../resources/shaders/particle.vs", "../resources/shaders/particle.fs");
    Shader terrainShader("../resources/shaders/terrain.vs", "../resources/shaders/terrain.fs");
    Shader skyboxShader("../resources/shaders/skybox.vs", "../resources/shaders/skybox.fs");
    
    vector<std::string> faces = {
        "../resources/textures/skybox/right.jpg",
        "../resources/textures/skybox/left.jpg",
        "../resources/textures/skybox/top.jpg",
        "../resources/textures/skybox/bottom.jpg",
        "../resources/textures/skybox/front.jpg",
        "../resources/textures/skybox/back.jpg"
    };
    SkyBox skybox(faces);

    // LightScene lightScene(glm::vec3(0.1f,0.1f,0.1f),{
    LightScene lightScene(glm::vec3(0.0f),{
        PointLight{glm::vec3(1000.0f,1000.0f,1000.0f),glm::vec3(1.0f,1.0f,1.0f),glm::vec3(1.0f,0.0f,0.0f)}
    });

    // WaterGun blueGun(100000,{0.0f,0.0f,-20.0f},{0.0f,1.0f,0.0f},{0.0f,0.464f,0.742f,0.2f},20.0f,0.1,5.0f,10000);
    // WaterGun purpleGun(100000,{-5.0f,0.0f,-20.0f},{1.0f,1.0f,0.0f},{0.464f,0.0f,0.742f,0.3f},10.0f,0.2,3.0f,10000);
    // Dust redHoli(100000,{-5.0f,0.0f,-20.0f},{0.464f,0.1f,0.242f,0.3f},
                    // {{-5.0f,-10.0f,-25.0f},{-5.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f}},
                    // {{0.0f,1.0f,0.0f},{5.0f,3.0f,0.0f},{-1.0,1.0f,0.0f}}
                    // ,{2.0f,10.0f,5.0f},
                    // 1.0f,20.0f,5000,2.0f);
    int terrainSize = 400;
    int waterSize = 4000;
    int terrainCount = 400;
    int terrainHeight = 100;
    Terrain ground(-terrainSize/2,-terrainSize/2,terrainSize,terrainCount,terrainHeight,{
                                        "../resources/textures/grass.png",
                                        "../resources/textures/water.jpg",
                                        "../resources/textures/rock.jpg",
                                        "../resources/textures/snow.jpg",
                                        },"../resources/textures/heightmap.png");
    // Terrain ground(-terrainSize/2,-terrainSize/2,terrainSize,terrainCount,terrainHeight,{"../resources/textures/blendMap.png", \
    //                                    "../resources/textures/grass.png", \
    //                                    "../resources/textures/grassFlowers.png", \
    //                                    "../resources/textures/mud.png", \
    //                                    "../resources/textures/path.png"},"../resources/textures/heightmap.png");

    cout<<"Min terrain height: "<<ground.min_terrain_height<<endl;
    cout<<"Average terrain height: "<<ground.average_terrain_height<<endl;
    cout<<"Max terrain height: "<<ground.max_terrain_height<<endl;

    camera.Position.y = ground.average_terrain_height*1.2; // set camera height correctly

    glm::vec3 skyColor{0.53, 0.81, 0.98};
    float seaLevel = 0.95*ground.average_terrain_height;
    float grassLevel = 1.1*ground.average_terrain_height;
    float mountainLevel = 1.25*ground.average_terrain_height;

    Water water(glm::vec3(-waterSize/2,seaLevel,-waterSize/2),waterSize,5,{"../resources/textures/water.jpg"});


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
        glClearColor(skyColor[0],skyColor[1],skyColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::vec3 cameraPos = camera.Position;
        glm::mat4 view = camera.GetViewMatrix();

        terrainShader.use();
        lightScene.configureLights(terrainShader);
        terrainShader.setFloat("seaLevel",seaLevel);
        terrainShader.setFloat("grassLevel",grassLevel);
        terrainShader.setFloat("mountainLevel",mountainLevel);

        terrainShader.setVec3("skyColor",skyColor);
        terrainShader.setFloat("shineDamper",1.0f);
        terrainShader.setFloat("reflectivity",0.0f);
        terrainShader.setVec3("viewPos",cameraPos);
        terrainShader.setMat4("projection",projection);
        terrainShader.setMat4("view",view);
        water.Draw(terrainShader);
        ground.Draw(terrainShader);

        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        // skybox.Draw();


        // particleShader.use();
        // particleShader.setMat4("projection", projection);
        // particleShader.setMat4("view", view);

        // // blueGun.Update(deltaTime,cameraPos);
        // // purpleGun.Update(deltaTime,cameraPos);
        // redHoli.Update(deltaTime,timePassed,cameraPos);

        // // blueGun.Draw(particleShader);
        // // purpleGun.Draw(particleShader);
        // redHoli.Draw(particleShader);

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
