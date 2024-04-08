#ifndef EDITORUI_H
#define EDITORUI_H

#include <glad/glad.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

class EditorUI
{
public:
	EditorUI(GLFWwindow* window);
	~EditorUI();
	void showEditorUI();
	void showDetailWindow(); //չʾ���������ui����
	void showFileContentWindow(const fs::path& path);//��Դ����
	void buildFileTree(const fs::path& path);//��ui�д���ָ���ļ��еĽṹ
	bool isObjFile(const std::string& file); //�ж��ļ��ǲ���.obj
private:
	//assets�����Ӧ��һЩ����
	int fileIdCount; //���νṹ�Ľڵ�
	const char* fileId; //Ŀ¼�ṹ�Ķ��ر�ʶ��
	fs::path assetsPath; //��ԴĿ¼��·��
};

#endif // !EDITORUI_H