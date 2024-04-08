#include "editor_ui.h"

EditorUI::EditorUI(GLFWwindow* window)
{
    //����imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext(); //����������
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // ������̿���
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    //// ������Ⱦ�����
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

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

    //��ʼ����ImGui
    ImGui::Begin("WurtEngine");
    ImGui::Indent();
    ImGui::Text("text");
    //ImGui::SliderInt("specuMi", &specuMi, 0, 1024);
    /*ImGui::SliderFloat3("ambientColor", &ambientColor.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("diffuseColor", &diffuseColor.x, 0.0f, 1.0f);
    ImGui::SliderFloat3("specularColor", &specularColor.x, 0.0f, 1.0f);*/
    
    //ImGui::SliderFloat3("dirLightPos", &dirLightPos.x, -10.0f, 10.0f);
    
    /*ImGui::InputInt("pointLightCount", &pointLightCount);
    for (int i = 0; i < pointLightCount; i++)
    {
        std::string temp1 = "pointLightPos[" + std::to_string(i) + "]";
        ImGui::SliderFloat3(temp1.c_str(), &pointLightPos[i].x, -10.0f, 10.0f);
        std::string temp2 = "pointLightAmbient[" + std::to_string(i) + "]";
        ImGui::SliderFloat3(temp2.c_str(), &pointLightAmbient[i].x, 0.0f, 1.0f);
    }*/
    /*ImGui::SliderFloat3("spotLightPos", &spotLightPos.x, -10.0f, 10.0f);
    ImGui::SliderFloat3("spotLightDir", &spotLightDir.x, -1.0f, 1.0f);*/
    
    ImGui::End();
    
}

void EditorUI::showFileContentWindow(const fs::path& path)
{
    fileIdCount = 0; //��Ϊ0

    ImGui::Begin("File Content");
    ImGui::SetWindowFontScale(1.5f); //��������Ŵ�
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
        if (file.is_directory()) //���ļ���
        {

            if (ImGui::TreeNodeEx((fileId + fileIdCount++), ImGuiTreeNodeFlags_None, file.path().filename().string().c_str()))
            {
                buildFileTree(file.path());
                ImGui::TreePop();
            }
        }
        else
        {
            if (isObjFile(file.path().string())) //obj�ļ������Ե�����������ڳ���ԭ�㴴��һ��
            {
                if (ImGui::Button(file.path().filename().string().c_str()))
                {
                    std::cout << "init obj" << std::endl; //������ģ��
                }
            }
            else
            {
                ImGui::Text(file.path().filename().string().c_str());
            }
        }
    }
}

//�ж��ļ��ĺ�׺�ǲ���.obj
bool EditorUI::isObjFile(const std::string& file)
{
    int dotPos = file.rfind('.'); //Ѱ�����һ��.��λ��
    if (dotPos != std::string::npos && dotPos != 0)
    {
        if (file.substr(dotPos + 1) == "obj")
            return true;
    }
    return false;
}