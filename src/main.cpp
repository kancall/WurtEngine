#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif // !STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "editor_ui.h"
#include "shader.h"
#include "camera.h"
#include "model.h"
#include "light.h"

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void scene(EditorData* mydata);

//datas
EditorData* mydata;
EditorUI* myUI;
unsigned int Model::cnt = 0;

Model* backpack;
Model* floorModel[4];
Model* pointlight;
Model* dirlight;

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 600;

//camera
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2; //鼠标上一次所在的位置
bool firstMouse = true; //重新开始一次摄像机的视角移动

float deltaTime = 0.0f; //每帧间隔时间
float lastFrame = 0.0f; //上一帧的时间

//light
int pointLightCount = 1;
std::vector<PointLight> pointLights;

//phong
int specuMi = 64;

//ui

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

    //在这里创建EditorUI类对象，然后使用
    mydata = new EditorData;
    myUI = new EditorUI(window, mydata);

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    Shader objectShader("src/modelLoad.vs", "src/modelLoad.fs");
    Shader lightShader("src/myrenderVs.vs", "src/lightRenderFs.fs");
    mydata->addNewMateial("objectShader", &objectShader);
    mydata->addNewMateial("lightShader", &lightShader);

    pointlight = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//cube//cube.obj");
    mydata->allModels[pointlight->ID] = pointlight;
    dirlight = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//sphere//sphere.obj");
    mydata->allModels[dirlight->ID] = dirlight;

    backpack = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//backpack//backpack.obj");
    mydata->allModels[backpack->ID] = backpack;
    for (int i = 0; i < 4; i++)
    {
        floorModel[i] = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//floor//floor.obj");
        //mydata->allModels[floorModel[i]->ID] = floorModel[i];
    }
    floorModel[0]->position.z -= 3.0f;
    floorModel[0]->position.y -= 10.0f;
    floorModel[0]->rotation.x += 90.0f;
    floorModel[1]->position.z -= 30.0f;
    floorModel[2]->position.x -= 30.0f;
    floorModel[2]->rotation.y -= 90.0f;
    floorModel[3]->position.x += 30.0f;
    floorModel[3]->rotation.y -= 90.0f;
    
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

        glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = mydata->camera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        /*model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));*/
        Shader* lightShader = mydata->materials["lightShader"];
        lightShader->use();
        lightShader->setMat4("proj", projection);
        lightShader->setMat4("view", view);
        for (int i = 0; i < mydata->pointLightCount; i++)
        {
            lightShader->setVec3("lightColor", mydata->pointLights[i].ambient);
            model = glm::mat4(1.0f);
            //model = glm::translate(model, mydata->pointLights[i].position);
            model = glm::translate(model, pointlight->position);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader->setMat4("model", model);
            pointlight->Draw(lightShader);
        }
        lightShader->setVec3("lightColor", mydata->dirLights[0].ambient);
        model = glm::mat4(1.0f);
        model = glm::translate(model, dirlight->position);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader->setMat4("model", model);
        dirlight->Draw(lightShader);

        scene(mydata);
        mydata->showNewModels();
        //渲染ui
        myUI->showEditorUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void scene(EditorData* mydata) //backpack myModel临时测试用，后续删除
{
    /*mydata->materials["lightShader"]->use();
    mydata->lightShaderData(mydata->materials["lightShader"], dirlight, true, 0);
    dirlight->Draw(mydata->materials["lightShader"]);
    mydata->lightShaderData(mydata->materials["lightShader"], pointlight, false, 0);
    pointlight->Draw(mydata->materials["lightShader"]);*/


    mydata->materials["objectShader"]->use();

    mydata->phongShaderData(mydata->materials["objectShader"], backpack);
    backpack->Draw(mydata->materials["objectShader"]);
    
    mydata->phongShaderData(mydata->materials["objectShader"], floorModel[0]);
    floorModel[0]->Draw(mydata->materials["objectShader"]);

    mydata->phongShaderData(mydata->materials["objectShader"], floorModel[1]);
    floorModel[1]->Draw(mydata->materials["objectShader"]);

    mydata->phongShaderData(mydata->materials["objectShader"], floorModel[2]);
    floorModel[2]->Draw(mydata->materials["objectShader"]);

    mydata->phongShaderData(mydata->materials["objectShader"], floorModel[3]);
    floorModel[3]->Draw(mydata->materials["objectShader"]);

    //objectShader->setVec3("viewPos", mydata->camera->Position);
    //objectShader->setFloat("material.shininess", specuMi);
    ////lights
    //objectShader->setVec3("dirLight.direction", mydata->dirLights[0].direction);
    //objectShader->setVec3("dirLight.ambient", mydata->dirLights[0].ambient);
    //objectShader->setVec3("dirLight.diffuse", mydata->dirLights[0].diffuse);
    //objectShader->setVec3("dirLight.specular", mydata->dirLights[0].specular);

    //objectShader->setInt("pointLightCount", mydata->pointLightCount);
    //for (int i = 0; i < mydata->pointLightCount; i++)
    //{
    //    std::string index = "pointLights[" + std::to_string(i) + "]";

    //    objectShader->setVec3(index + ".position", mydata->pointLights[0].position);
    //    objectShader->setFloat(index + ".constant", 1.0f);
    //    objectShader->setFloat(index + ".linear", 0.09f);
    //    objectShader->setFloat(index + ".quadratic", 0.032f);
    //    objectShader->setVec3(index + ".ambient", mydata->pointLights[0].ambient);
    //    objectShader->setVec3(index + ".diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    //    objectShader->setVec3(index + ".specular", glm::vec3(1.0f, 1.0f, 1.0f));
    //}
    ////矩阵们
    //glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //glm::mat4 view = mydata->camera->GetViewMatrix();
    //objectShader->setMat4("projection", projection);
    //objectShader->setMat4("view", view);
    //glm::mat4 model = glm::mat4(1.0f);
    //model = glm::translate(model, backpack->position);
    //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    //objectShader->setMat4("model", model);
    //背包模型draw
    //backpack->Draw(objectShader);

    /*model = glm::mat4(1.0f);
    model = glm::translate(model, floorModel->position);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    objectShader->setMat4("model", model);
    floorModel->Draw(objectShader);*/

    //Shader* lightShader= mydata->materials["lightShader"];
    //lightShader->use();
    //lightShader->setMat4("proj", projection);
    //lightShader->setMat4("view", view);
    //for (int i = 0; i < mydata->pointLightCount; i++)
    //{
    //    lightShader->setVec3("lightColor", mydata->pointLights[i].ambient);

    //    model = glm::mat4(1.0f);
    //    model = glm::translate(model, mydata->pointLights[i].position);
    //    model = glm::scale(model, glm::vec3(0.2f));
    //    lightShader->setMat4("model", model);
    //    //方块模型draw
    //}
    //lightShader->setVec3("lightColor", mydata->dirLights[0].ambient);

    //model = glm::mat4(1.0f);
    //model = glm::translate(model, mydata->dirLights[0].position);
    //model = glm::scale(model, glm::vec3(0.2f));
    //lightShader->setMat4("model", model);
    ////方块模型draw
}

void processInput(GLFWwindow* window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) //按下鼠标左键
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (myUI->selectedModelId != -1)
        {
            myUI->lastSelected = myUI->selectedModelId;
        }
        myUI->selectedModelId = mydata->getSelectId(xpos, ypos);

        if (myUI->selectedModelId != -1)
            std::cout << "select model id : " << myUI->selectedModelId << std::endl;

    }
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//向上
    {
        mydata->camera->ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//向下
    {
        mydata->camera->ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//向左
    {
        mydata->camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//向右
    {
        mydata->camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)//向前
    {
        mydata->camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//向后
    {
        mydata->camera->ProcessKeyboard(BACKWARD, deltaTime);
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

    mydata->camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    mydata->camera->ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
