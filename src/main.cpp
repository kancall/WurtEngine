﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "backends/imgui_impl_glfw.cpp"
#include "backends/imgui_impl_opengl3.cpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <string>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderUI();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//camera
Camera camera(glm::vec3(1.0, 0.0, 6.0));
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2; //鼠标上一次所在的位置
bool firstMouse = true; //重新开始一次摄像机的视角移动

float deltaTime = 0.0f; //每帧间隔时间
float lastFrame = 0.0f; //上一帧的时间

//light
int pointLightCount = 1;
glm::vec3 dirLightPos(1.0, 1.0, 1.0); //平行光的光源位置

glm::vec3 spotLightPos(1.2f, 0.0f, 0.0f); //聚光灯的位置
glm::vec3 spotLightDir(0.0f, 0.0f, -1.0f); //聚光灯的方向

glm::vec3 pointLightPos[10]; //点光源的位置
glm::vec3 pointLightAmbient[10];

glm::vec3 ambientColor(-0.2f);
glm::vec3 diffuseColor(1.0f);
glm::vec3 specularColor(1.0f);

//phong
int specuMi = 64;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


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
    glfwSwapInterval(1);
    gladLoadGL();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //GLFW_CURSOR_DISABLED是不显示鼠标
    //加载glad库
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //加载imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); //创建上下文
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 允许键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    //// 设置渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    Shader objectShader("src/modelLoad.vs", "src/modelLoad.fs");
    Model myModel("E://vs c++ practice//WurtEngine//WurtEngine//res//model//backpack//backpack.obj");
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        // input
        // -----
        processInput(window);

        // 清空缓存
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        objectShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	
        objectShader.setMat4("model", model);
        myModel.Draw(objectShader);

        //渲染ui
        renderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void renderUI()
{
    //ui绘制
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    {
        //开始绘制ImGui
        ImGui::Begin("WurtEngine"); 
        ImGui::Indent();
        ImGui::SliderInt("specuMi", &specuMi, 0, 1024);
        ImGui::SliderFloat3("ambientColor", &ambientColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("diffuseColor", &diffuseColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("specularColor", &specularColor.x, 0.0f, 1.0f);

        ImGui::SliderFloat3("dirLightPos", &dirLightPos.x, -10.0f, 10.0f);

        ImGui::InputInt("pointLightCount", &pointLightCount);
        for (int i = 0; i < pointLightCount; i++)
        {
            string temp1 = "pointLightPos[" + to_string(i) + "]";
            ImGui::SliderFloat3(temp1.c_str(), &pointLightPos[i].x, -10.0f, 10.0f);
            string temp2 = "pointLightAmbient[" + to_string(i) + "]";
            ImGui::SliderFloat3(temp2.c_str(), &pointLightAmbient[i].x, 0.0f, 1.0f);
        }

        ImGui::SliderFloat3("spotLightPos", &spotLightPos.x, -10.0f, 10.0f);
        ImGui::SliderFloat3("spotLightDir", &spotLightDir.x, -1.0f, 1.0f);

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//向上
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//向下
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//向左
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//向右
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)//向前
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//向后
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS) //按下鼠标左键才可以移动摄像头视角
    {
        firstMouse = true;
        return;
    }
    if (firstMouse) //重新开始一次移动回调
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX, yoffset = lastY - ypos; //屏幕起点坐标在左上角
    lastX = xpos, lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

