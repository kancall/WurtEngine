#ifndef EDITORUI_H
#define EDITORUI_H

#include <glad/glad.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "light.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include "editor_data.h"
namespace fs = std::filesystem;

class EditorUI
{
public:
	EditorUI(GLFWwindow* window, EditorData* data);
	~EditorUI();
	void test(); //测试界面，显示物体的信息，并且切换物体的时候信息也会切换
	void showEditorUI();
	void showDetailWindow(); //展示物体信息的ui界面
	void showFileContentWindow(const fs::path& path);//资源界面
	void buildFileTree(const fs::path& path);//在ui中创建指定文件夹的结构
	bool isObjFile(const std::string& file); //判断文件是不是.obj
private:
	//assets界面对应的一些内容
	int fileIdCount; //树形结构的节点
	const char* fileId; //目录结构的独特标识符
	fs::path assetsPath; //资源目录的路径

	EditorData* data;
	unsigned int selectedModelId; //当前被选中的物体
};

#endif // !EDITORUI_H