#include "editor_ui.h"

EditorUI::EditorUI(GLFWwindow* window, EditorData* data)
{
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

    this->data = data;
    //file content init
    fileIdCount = 0;
    fileId = "MyContentFileId";
    assetsPath = "E://vs c++ practice//WurtEngine//WurtEngine//res";

}

EditorUI::~EditorUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorUI::showEditorUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    showDetailWindow();
    showFileContentWindow(assetsPath);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void EditorUI::showDetailWindow()
{

    //开始绘制ImGui
    ImGui::Begin("WurtEngine");
    ImGui::Indent();
    ImGui::InputInt("dirLightCount", &data->dirLightCount);
    ImGui::InputInt("pointLightCount", &data->pointLightCount);
    ImGui::InputInt("spotLightCount", &data->spotLightCount);
    //ImGui::SliderInt("specuMi", &specuMi, 0, 1024);
    /*ImGui::SliderFloat3("ambientColor", &ambientColor.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("diffuseColor", &diffuseColor.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("specularColor", &specularColor.x, 0.0f, 1.0f);*/
    
    //ImGui::SliderFloat3("dirLightPos", &dirLightPos.x, -10.0f, 10.0f);
    if (data->pointLightCount > data->pointLights.size())
    {
        std::vector<PointLight> temp(data->pointLightCount - data->pointLights.size());
        data->pointLights.insert(data->pointLights.end(), temp.begin(), temp.end());
    }
    for (int i = 0; i < data->pointLightCount; i++)
    {
        std::string temp1 = "pointLightPos[" + std::to_string(i) + "]";
        ImGui::SliderFloat3(temp1.c_str(), &data->pointLights[i].position.x, -10.0f, 10.0f);
        std::string temp2 = "pointLightAmbient[" + std::to_string(i) + "]";
        ImGui::SliderFloat3(temp2.c_str(), &data->pointLights[i].ambient.x, 0.0f, 1.0f);
    }
    /*ImGui::SliderFloat3("spotLightPos", &spotLightPos.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("spotLightDir", &spotLightDir.x, -1.0f, 1.0f);*/
    
    ImGui::End();
    
}

void EditorUI::showFileContentWindow(const fs::path& path)
{
    fileIdCount = 0; //设为0

    ImGui::Begin("File Content");
    ImGui::SetWindowFontScale(1.5f); //窗口字体放大
    if (ImGui::TreeNode("assets"))
    {
        buildFileTree(path);
        ImGui::TreePop();
    }
    ImGui::End();
}

void EditorUI::buildFileTree(const fs::path& path)
{
    if (!fs::exists(path))
    {
        ImGui::Text("error! not exist %s", path);
        return;
    }

    for (const auto& file : fs::directory_iterator(path))
    {
        if (file.is_directory()) //是文件夹
        {

            if (ImGui::TreeNodeEx((fileId + fileIdCount++), ImGuiTreeNodeFlags_None, file.path().filename().string().c_str()))
            {
                buildFileTree(file.path());
                ImGui::TreePop();
            }
        }
        else
        {
            if (isObjFile(file.path().string())) //obj文件。可以点击，点击后会在场景原点创建一个
            {
                if (ImGui::Button(file.path().filename().string().c_str()))
                {
                    std::cout << "init obj" << std::endl; //创建该模型
                }
            }
            else
            {
                ImGui::Text(file.path().filename().string().c_str());
            }
        }
    }
}

//判断文件的后缀是不是.obj
bool EditorUI::isObjFile(const std::string& file)
{
    int dotPos = file.rfind('.'); //寻找最后一个.的位置
    if (dotPos != std::string::npos && dotPos != 0)
    {
        if (file.substr(dotPos + 1) == "obj")
            return true;
    }
    return false;
}