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
	void test(); //���Խ��棬��ʾ�������Ϣ�������л������ʱ����ϢҲ���л�
	void showEditorUI();
	void showDetailWindow(); //չʾ������Ϣ��ui����
	void showFileContentWindow(const fs::path& path);//��Դ����
	void buildFileTree(const fs::path& path);//��ui�д���ָ���ļ��еĽṹ
	bool isObjFile(const std::string& file); //�ж��ļ��ǲ���.obj
private:
	//assets�����Ӧ��һЩ����
	int fileIdCount; //���νṹ�Ľڵ�
	const char* fileId; //Ŀ¼�ṹ�Ķ��ر�ʶ��
	fs::path assetsPath; //��ԴĿ¼��·��

	EditorData* data;
	unsigned int selectedModelId; //��ǰ��ѡ�е�����
};

#endif // !EDITORUI_H