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
	int dirLightCount; //平行光源的数量
	int pointLightCount;
	int spotLightCount;

	std::vector<DirLight> dirLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	std::unordered_map<unsigned int, Model*> gModels;//存储场景所有的模型
	std::unordered_map<unsigned int, Model*> models; //存储新添加的模型
	std::unordered_map<std::string, Shader*> materials;
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
	//创建新的模型  只用于存储新添加的模型，场景中默认设置好的模型不用管
	Model* addNewModel(std::string const& path)
	{
		//1.添加模型
		Model* model = new Model(path);
		models[model->ID] = model;

		return model;
	}
	//获取被选中物体的id
	unsigned int getSelectId()
	{
		return 1;
	}
	//显示所有的新增模型
	void showNewModels()
	{
		//2.用默认shader激活模型
		materials["default"]->use();
		for (std::unordered_map<unsigned int, Model*>::iterator it = models.begin(); it != models.end(); it++)
		{
			//绑定数据
			defalutShaderData(materials["default"], it->second);
			//显示模型
			it->second->Draw(materials["default"]);
		}
	}

	//返回指定id的model
	Model* getSelectedModelData(unsigned int id)
	{
		return models[id];
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

	void defalutShaderData(Shader* defaultShader, Model* myModel)
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
		//矩阵们
		glm::mat4 projection = glm::perspective(glm::radians(this->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
		glm::mat4 view = this->camera->GetViewMatrix();
		defaultShader->setMat4("projection", projection);
		defaultShader->setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, myModel->position);
		model = glm::rotate(model, glm::radians(myModel->rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(myModel->rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(myModel->rotation.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, myModel->scale);

		defaultShader->setMat4("model", model);
	}

private:

};

#endif // !EDITOR_DATA_H
