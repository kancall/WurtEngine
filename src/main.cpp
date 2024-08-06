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
#include <map>
namespace fs = std::filesystem;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void scene(EditorData* mydata);
unsigned int loadTexture(char const* path);
unsigned int loadCubeTexture(std::vector<std::string>& textures_faces);

//datas
EditorData* mydata;
EditorUI* myUI;
unsigned int Model::cnt = 0;

Model* backpack;
Model* pointlight;
Model* dirlight;
Model* person;
Model* zhongli;
Model* floorModel;

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

    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    //启用混合，并设置混合函数
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float planeVertices[] = {
         30.0f, -0.5f,  20.0f,  4.0f, 0.0f,
        -30.0f, -0.5f,  20.0f,  0.0f, 0.0f,
        -30.0f, -0.5f, -20.0f,  0.0f, 4.0f,

         30.0f, -0.5f,  20.0f,  4.0f, 0.0f,
        -30.0f, -0.5f, -20.0f,  0.0f, 4.0f,
         30.0f, -0.5f, -20.0f,  4.0f, 4.0f
    };
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    float transparentVertices[] = {
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    std::vector<glm::vec3> windows
    {
        glm::vec3(-1.5f, -2.0f, -0.48f),
        glm::vec3(1.5f, -2.0f, 0.51f),
        glm::vec3(0.0f, -2.0f, 0.7f),
        glm::vec3(-0.3f, -2.0f, -2.3f),
        glm::vec3(0.5f, -2.0f, -0.6f)
    };

    unsigned int grassVAO, grassVBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), &transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

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
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    //在这里创建EditorUI类对象，然后使用
    mydata = new EditorData;
    myUI = new EditorUI(window, mydata);


    Shader objectShader("modelLoad.vs", "modelLoad.fs");
    Shader lightShader("myrenderVs.vs", "lightRenderFs.fs");
    Shader floorShader("floorVs.vs", "floorFs.fs");
    Shader grassShader("grassVs.vs", "grassFs.fs");
    Shader cubemapShader("cubemapVs.vs", "cubemapFs.fs");
    mydata->addNewMateial("objectShader", &objectShader);
    mydata->addNewMateial("lightShader", &lightShader);
    mydata->addNewMateial("floorShader", &floorShader);
    mydata->addNewMateial("grassShader", &grassShader);
    mydata->addNewMateial("cubemapShader", &cubemapShader);

    unsigned int floorTexture = loadTexture("E://vs c++ practice//WurtEngine//WurtEngine//res//texture//diffuse.jpg"); 
    unsigned int grassTexture = loadTexture("E://vs c++ practice//WurtEngine//WurtEngine//res//texture//transparent_window.png");
    std::vector<std::string> faces
    {
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//right.jpg",
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//left.jpg",
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//bottom.jpg",
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//top.jpg",
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//front.jpg",
        "E://vs c++ practice//WurtEngine//WurtEngine//res//skybox//sea//back.jpg"
    };
    unsigned int cubemapTexture = loadCubeTexture(faces);

    pointlight = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//cube//cube.obj");
    mydata->allModels[pointlight->ID] = pointlight;
    dirlight = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//sphere//sphere.obj");
    mydata->allModels[dirlight->ID] = dirlight;
    backpack = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//backpack//backpack.obj");
    mydata->allModels[backpack->ID] = backpack;
    person = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//person.fbx");
    person->scale = glm::vec3(0.05f);
    mydata->allModels[person->ID] = person;
    floorModel = new Model("E://vs c++ practice//WurtEngine//WurtEngine//res//model//floor//floor.obj");
    floorModel->position = glm::vec3(0.0, -5.0, 0.0);
    floorModel->rotation = glm::vec3(90.0f, 0.0, 0.0);
    mydata->allModels[floorModel->ID] = floorModel;
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


        // ---------------------
        //先绘制所有不透明的物体
        // ---------------------


        //floor
        {
            glActiveTexture(GL_TEXTURE0); 
            glBindTexture(GL_TEXTURE_2D, floorTexture);
            mydata->materials["floorShader"]->use();
            mydata->materials["floorShader"]->setInt("texture_diffuse1", 0);

            //矩阵们
            glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
            glm::mat4 view = mydata->camera->GetViewMatrix();
            mydata->materials["floorShader"]->setMat4("projection", projection);
            mydata->materials["floorShader"]->setMat4("view", view);
            glm::mat4 model = glm::mat4(1.0f);
            //下
            model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0));
            /*model = glm::rotate(model, glm::radians(myModel->rotation.x), glm::vec3(1.0, 0.0, 0.0));
            model = glm::rotate(model, glm::radians(myModel->rotation.y), glm::vec3(0.0, 1.0, 0.0));
            model = glm::rotate(model, glm::radians(myModel->rotation.z), glm::vec3(0.0, 0.0, 1.0));*/
            model = glm::scale(model, glm::vec3(1.0f));
            mydata->materials["floorShader"]->setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //前
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0, 17.0f, -19));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            mydata->materials["floorShader"]->setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //右
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(30, 17.0f, -10));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
            mydata->materials["floorShader"]->setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //左
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-30, 17.0f, -10));
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
            mydata->materials["floorShader"]->setMat4("model", model);
            glBindVertexArray(planeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        {
        //    mydata->materials["objectShader"]->use();
        //    mydata->materials["objectShader"]->setInt("texture_diffuse1", 0);
        //    mydata->materials["objectShader"]->setVec3("viewPos", mydata->camera->Position);
        //    mydata->materials["objectShader"]->setFloat("material.shininess", mydata->specuMi);
        //    //lights
        //    mydata->materials["objectShader"]->setVec3("dirLight.direction", mydata->dirLights[0].direction);
        //    mydata->materials["objectShader"]->setVec3("dirLight.ambient", mydata->dirLights[0].ambient);
        //    mydata->materials["objectShader"]->setVec3("dirLight.diffuse", mydata->dirLights[0].diffuse);
        //    mydata->materials["objectShader"]->setVec3("dirLight.specular", mydata->dirLights[0].specular);

        //    mydata->materials["objectShader"]->setInt("pointLightCount", mydata->pointLightCount);
        //    for (int i = 0; i < mydata->pointLightCount; i++)
        //    {
        //        std::string index = "pointLights[" + std::to_string(i) + "]";

        //        mydata->materials["objectShader"]->setVec3(index + ".position", mydata->pointLights[0].position);
        //        mydata->materials["objectShader"]->setFloat(index + ".constant", 1.0f);
        //        mydata->materials["objectShader"]->setFloat(index + ".linear", 0.09f);
        //        mydata->materials["objectShader"]->setFloat(index + ".quadratic", 0.032f);
        //        mydata->materials["objectShader"]->setVec3(index + ".ambient", mydata->pointLights[0].ambient);
        //        mydata->materials["objectShader"]->setVec3(index + ".diffuse", mydata->pointLights[0].diffuse);
        //        mydata->materials["objectShader"]->setVec3(index + ".specular", mydata->pointLights[0].specular);
        //    }
        //    //矩阵们
        //    glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
        //    glm::mat4 view = mydata->camera->GetViewMatrix();
        //    mydata->materials["objectShader"]->setMat4("projection", projection);
        //    mydata->materials["objectShader"]->setMat4("view", view);
        //    glm::mat4 model = glm::mat4(1.0f);
        //    model = glm::translate(model, glm::vec3(0.0f));
        //    /*model = glm::rotate(model, glm::radians(myModel->rotation.x), glm::vec3(1.0, 0.0, 0.0));
        //    model = glm::rotate(model, glm::radians(myModel->rotation.y), glm::vec3(0.0, 1.0, 0.0));
        //    model = glm::rotate(model, glm::radians(myModel->rotation.z), glm::vec3(0.0, 0.0, 1.0));*/
        //    model = glm::scale(model, glm::vec3(1.0f));
        //    mydata->materials["objectShader"]->setMat4("model", model);

        //    glBindVertexArray(planeVAO);
        //    glBindTexture(GL_TEXTURE_2D, floorTexture);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //    glBindVertexArray(0);
        }

        glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = mydata->camera->GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        Shader* lightShader = mydata->materials["lightShader"];
        lightShader->use();
        lightShader->setMat4("proj", projection);
        lightShader->setMat4("view", view);
        for (int i = 0; i < mydata->pointLightCount; i++)
        {
            lightShader->setVec3("lightColor", mydata->pointLights[i].ambient);
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointlight->position); //在imgui中改的是模型的position，所以这里要用模型的position，然后根据模型的position改一下data中的数据
            mydata->pointLights[i].position = pointlight->position;
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


        // ---------------------
        //对所有透明的物体排序
        // ---------------------
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(mydata->camera->Position - windows[i]);
            sorted[distance] = windows[i];
        }
        

        // ---------------------
        //按顺序绘制所有透明的物体
        // ---------------------
        //grass
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, grassTexture);
            glBindVertexArray(grassVAO);
            mydata->materials["grassShader"]->use();
            mydata->materials["grassShader"]->setInt("texture_diffuse1", 0);

            //矩阵们
            glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
            glm::mat4 view = mydata->camera->GetViewMatrix();
            mydata->materials["grassShader"]->setMat4("projection", projection);
            mydata->materials["grassShader"]->setMat4("view", view);

            glm::mat4 model = glm::mat4(1.0f);
            /*model = glm::translate(model, glm::vec3(3.0f, -2.0f, 0));
            model = glm::scale(model, glm::vec3(1.0f));
            mydata->materials["grassShader"]->setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);*/

            for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, it->second);
                model = glm::scale(model, glm::vec3(1.0f));
                mydata->materials["grassShader"]->setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        //天空盒
        {
            glDepthFunc(GL_LEQUAL);
            mydata->materials["cubemapShader"]->use();
            mydata->materials["cubemapShader"]->setInt("skybox", 0);
            //矩阵们
            glm::mat4 projection = glm::perspective(glm::radians(mydata->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
            glm::mat4 view = glm::mat4(glm::mat3(mydata->camera->GetViewMatrix()));
            mydata->materials["cubemapShader"]->setMat4("projection", projection);
            mydata->materials["cubemapShader"]->setMat4("view", view);

            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS);
        }
        //渲染ui
        myUI->showEditorUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void scene(EditorData* mydata)
{
    /*mydata->materials["lightShader"]->use();
    mydata->lightShaderData(mydata->materials["lightShader"], dirlight, true, 0);
    dirlight->Draw(mydata->materials["lightShader"]);
    mydata->lightShaderData(mydata->materials["lightShader"], pointlight, false, 0);
    pointlight->Draw(mydata->materials["lightShader"]);*/


    mydata->materials["objectShader"]->use();

    /*mydata->phongShaderData(mydata->materials["objectShader"], backpack);
    backpack->Draw(mydata->materials["objectShader"]);*/

    mydata->phongShaderData(mydata->materials["objectShader"], person);
    person->Draw(mydata->materials["objectShader"]);

    mydata->phongShaderData(mydata->materials["objectShader"], floorModel);
    floorModel->Draw(mydata->materials["objectShader"]);

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
        //如果点击在ui上就结束
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse)
        {
            return;
        }
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
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubeTexture(std::vector<std::string>&textures_faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < textures_faces.size(); i++)
    {
        unsigned char* data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
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