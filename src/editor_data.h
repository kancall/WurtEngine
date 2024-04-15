#ifndef EDITOR_DATA_H
#define EDITOR_DATA_H

#include <GL/glut.h>

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

	int specuMi; //高光材质的幂
	std::vector<DirLight> dirLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	std::unordered_map<unsigned int, Model*> allModels;//存储场景所有的模型
	std::unordered_map<unsigned int, Model*> models; //存储新添加的模型
	std::unordered_map<std::string, Shader*> materials;
	EditorData() 
	{
		materials["default"] = new Shader("src/default.vs", "src/default.fs");
		
		camera = new Camera(glm::vec3(0.0, 0.0, 6.0));
		
		specuMi = 64;
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
		allModels[model->ID] = model; //存储所有物体，新创建的物体也要加进来

		return model;
	}
	//转换鼠标位置到3维空间
	glm::vec3 getWorldPos(int xpos, int ypos, glm::mat4 view, glm::mat4 proj)
	{
		GLdouble modelMatrix[16] = {
			view[0][0],view[0][1], view[0][2], view[0][3],
			view[1][0], view[1][1], view[1][2], view[1][3],
			view[2][0], view[2][1], view[2][2], view[2][3],
			view[3][0], view[3][1], view[3][2], view[3][3]
		};
		GLdouble projMatrix[16] = {
			proj[0][0], proj[0][1], proj[0][2], proj[0][3],
			proj[1][0], proj[1][1], proj[1][2], proj[1][3],
			proj[2][0], proj[2][1], proj[2][2], proj[2][3],
			proj[3][0], proj[3][1], proj[3][2], proj[3][3]
		};
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		GLdouble objx, objy, objz;
		GLfloat windowx, windowy, windowz;
		float mouse_x = xpos;
		float mouse_y = ypos;
		windowx = (float)mouse_x;
		windowy = (float)viewport[3] - (float)mouse_y;
		glReadBuffer(GL_BACK);
		glReadPixels(mouse_x, int(windowy), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &windowz);
		gluUnProject(windowx, windowy, windowz, modelMatrix, projMatrix, viewport, &objx, &objy, &objz);
		
		glm::vec3 res = glm::vec3(objx, objy, objz);
		return res;
	}
	
	int getSelectId(int xpos, int ypos)
	{
		//1.进行坐标转换，把屏幕坐标转换为世界坐标
		glm::mat4 projection = glm::perspective(glm::radians(this->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f);
		glm::mat4 view = this->camera->GetViewMatrix();
		glm::vec3 worldPos = getWorldPos(xpos, ypos, view, projection);
		//2.将转换后得到的坐标和摄像机坐标相连，得到一条射线
		//3.使用射线，一个个遍历模型，再在模型中一个个遍历三角形，和射线进行判断有没有交点，如果有就选中模型了，返回模型id
		//目前遇到的问题：因为是遍历数组一个个判断，如果有两个模型一前一后，如果先判断后面那个模型、并且有交点，那么其实实际上应该选前面那个，现实是选了后面的。可以通过深度值来判断一下
		for (std::unordered_map<unsigned int, Model*>::iterator it = allModels.begin(); it != allModels.end(); it++)
		{
			if (it->second->pickTrace(worldPos, this->camera->Position))
			{
				return it->first;
			}
		}

		return -1;
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
		return allModels[id];
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

	void lightShaderData(Shader* lightShader, Model* mymodel, bool isDirLight, int i) //后续可以优化一下，给light添加一个enum，根据enum判断是哪个灯类型
	{
		glm::mat4 projection = glm::perspective(glm::radians(this->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f);
		glm::mat4 view = this->camera->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		if (isDirLight)
		{
			model = glm::translate(model, this->dirLights[i].position);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader->setVec3("lightColor", this->dirLights[i].ambient);
		}
		else
		{
			model = glm::translate(model, this->pointLights[i].position);
			model = glm::scale(model, glm::vec3(0.2f));
			lightShader->setVec3("lightColor", this->pointLights[i].ambient);
		}
		lightShader->setMat4("proj", projection);
		lightShader->setMat4("view", view);
		lightShader->setMat4("model", model);

	}

	void phongShaderData(Shader* phongShader, Model* myModel)
	{
		phongShader->setVec3("viewPos", this->camera->Position);
		phongShader->setFloat("material.shininess", this->specuMi);
		//lights
		phongShader->setVec3("dirLight.direction", this->dirLights[0].direction);
		phongShader->setVec3("dirLight.ambient", this->dirLights[0].ambient);
		phongShader->setVec3("dirLight.diffuse", this->dirLights[0].diffuse);
		phongShader->setVec3("dirLight.specular", this->dirLights[0].specular);

		phongShader->setInt("pointLightCount", this->pointLightCount);
		for (int i = 0; i < this->pointLightCount; i++)
		{
			std::string index = "pointLights[" + std::to_string(i) + "]";

			phongShader->setVec3(index + ".position", this->pointLights[0].position);
			phongShader->setFloat(index + ".constant", 1.0f);
			phongShader->setFloat(index + ".linear", 0.09f);
			phongShader->setFloat(index + ".quadratic", 0.032f);
			phongShader->setVec3(index + ".ambient", this->pointLights[0].ambient);
			phongShader->setVec3(index + ".diffuse", this->pointLights[0].diffuse);
			phongShader->setVec3(index + ".specular", this->pointLights[0].specular);
		}
		//矩阵们
		glm::mat4 projection = glm::perspective(glm::radians(this->camera->Fov), (float)1000 / (float)600, 0.1f, 100.0f); //！注意，这里的width以后需要改成变量
		glm::mat4 view = this->camera->GetViewMatrix();
		phongShader->setMat4("projection", projection);
		phongShader->setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, myModel->position);
		model = glm::rotate(model, glm::radians(myModel->rotation.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(myModel->rotation.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(myModel->rotation.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, myModel->scale);
		phongShader->setMat4("model", model);
	}
private:

};

#endif // !EDITOR_DATA_H
