#ifndef EDITOR_DATA_H
#define EDITOR_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "light.h"
#include "model.h"
#include "shader.h"
#include "camera.h"

#include <iostream>
#include <vector>
#include <unordered_map>

class EditorData
{
public:
	Camera* camera;
	//light
	int dirLightCount; //ƽ�й�Դ������
	int pointLightCount;
	int spotLightCount;

	std::vector<DirLight> dirLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	std::vector<Model> modelss; //��ʱ�ã���֪��Ϊʲô��Model*�ͻ����
	std::unordered_map<unsigned int, Model*> models; //�洢����ӵ�ģ��  Model*�����д��󣬴�����
	//std::unordered_map<std::string, Model> models; //ֻ�洢�¼ӵ�
	std::unordered_map<std::string, Shader*> materials;
	std::unordered_map<int, Model> gModels;//�洢�������е�ģ��
	EditorData() 
	{
		materials["default"] = new Shader("src/default.vs", "src/default.fs");
		
		camera = new Camera(glm::vec3(1.0, 0.0, 6.0));
		
		//light
		dirLightCount = 1;
		pointLightCount = 1;
		spotLightCount = 0;
		dirLights.resize(dirLightCount);
		pointLights.resize(pointLightCount);
		spotLights.resize(spotLightCount);

	}
	~EditorData() {};
	//�����µ�ģ��  ֻ���ڴ洢����ӵ�ģ�ͣ�������Ĭ�����úõ�ģ�Ͳ��ù�
	void addNewModel(std::string const& path)
	{
		//1�����ģ��
		Model model(path);
		models[model.ID] = &model;
		modelss.push_back(model);

		//2.��Ĭ��shader����ģ��
		//materials["default"]->use();
		////������
		//defalutShaderData(materials["default"]);
		//model.Draw(materials["default"]);
	}
	//��ȡ��ѡ�������id
	unsigned int getSelectId()
	{
		return 1;
	}
	//��ʾ���е�����ģ��
	void showNewModels()
	{
		//2.��Ĭ��shader����ģ��
		materials["default"]->use();
		//������
		defalutShaderData(materials["default"]);
		/*for (std::unordered_map<unsigned int, Model*>::iterator it = models.begin(); it != models.end(); it++)
		{
			it->second->Draw(materials["default"]);
		}*/
		for (int i = 1; i < modelss.size(); i++)
		{
			modelss[i].Draw(materials["default"]);
		}
	}

	//����ָ��id��model
	Model* getSelectedModelData(unsigned int id)
	{
		return models[id];
	}
	//temp!!!!!!!!!!!!!!!!!�����ú���ɾ��
	Model getSelectedModelDataTemp(unsigned int id)
	{
		return modelss[0];
	}
	void addNewMateial(std::string const& name, Shader* shader)
	{
		if (materials.count(name))
		{
			printf("name conflict!");
			return;
		}
		materials[name] = shader;
	}

	void defalutShaderData(Shader* defaultShader)
	{
		defaultShader->setVec3("viewPos", this->camera->Position);
		//lights
		defaultShader->setVec3("dirLight.direction", this->dirLights[0].direction);
		defaultShader->setVec3("dirLight.ambient", this->dirLights[0].ambient);
		defaultShader->setInt("pointLightCount", this->pointLightCount);
		for (int i = 0; i < this->pointLightCount; i++)
		{
			std::string index = "pointLights[" + std::to_string(i) + "]";

			defaultShader->setVec3(index + ".position", this->pointLights[0].position);
			defaultShader->setFloat(index + ".constant", 1.0f);
			defaultShader->setFloat(index + ".linear", 0.09f);
			defaultShader->setFloat(index + ".quadratic", 0.032f);
			defaultShader->setVec3(index + ".ambient", this->pointLights[0].ambient);
		}
		//������
		glm::mat4 projection = glm::perspective(glm::radians(this->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //��ע�⣬�����width�Ժ���Ҫ�ĳɱ���
		glm::mat4 view = this->camera->GetViewMatrix();
		defaultShader->setMat4("projection", projection);
		defaultShader->setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		defaultShader->setMat4("model", model);
	}

private:

};

#endif // !EDITOR_DATA_H
