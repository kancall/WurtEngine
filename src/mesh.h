#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <iostream>
#include <string>
#include <vector>

//存储向量
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
//存储贴图
struct Texture
{
	unsigned int id; //纹理图 在opengl中对应的缓冲区id
	std::string type; //纹理类型，格式为texture_xxx
	std::string path;
};
//网格
class Mesh
{
public:
	//网格数据
	std::vector<Vertex> vertices; //顶点
	std::vector<unsigned int> indices; //网格索引
	std::vector<Texture> textures; //纹理
	unsigned int VAO;

	//初始化网格数据
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		setUpMesh();
	}
	void Draw(Shader* shader)
	{
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); //激活纹理

			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		//绘制网格
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0); //绘制
		glBindVertexArray(0);

		//glActiveTexture(GL_TEXTURE0);
	}

	bool pickTrace(glm::vec3 direc, glm::vec3 cameraPos, glm::vec3 modelWorldPos)
	{
		//逐片元判断有没有交点，如果有返回true
		for (int i = 0; i < indices.size(); i+=3)
		{
			glm::vec3 v1 = vertices[indices[i]].Position + modelWorldPos;
			glm::vec3 v2 = vertices[indices[i + 1]].Position + modelWorldPos;
			glm::vec3 v3 = vertices[indices[i + 2]].Position + modelWorldPos;

			glm::vec3 dir1 = v2 - v1;
			glm::vec3 dir2 = v3 - v1;
			glm::vec3 n = glm::normalize(glm::cross(dir1, dir2));

			//先判断射线和三角形所在面有无交点
			glm::vec3 eyeview = cameraPos - direc;
			double t = glm::dot(v1 - cameraPos, n) / glm::dot(n, eyeview);
			if (t >= 0)
				continue;
			//再判断该交点是否在三角形内部
			glm::vec3 p = cameraPos + glm::vec3(eyeview.x * t, eyeview.y * t, eyeview.z * t);
			glm::vec3 n1 = glm::normalize(glm::cross(p - v1, v2 - v1));
			glm::vec3 n2 = glm::normalize(glm::cross(p - v2, v3 - v2));
			glm::vec3 n3 = glm::normalize(glm::cross(p - v3, v1 - v3));
			if (!(glm::dot(n1, n2) > 0 && glm::dot(n1, n3) > 0))
				continue;

			return true;
		}
		return false;
	}
private:
	//顶点数据
	unsigned int VBO, EBO;

	//绑定顶点数据
	void setUpMesh()
	{
		glGenVertexArrays(1, &VAO);//顶点数组
		glGenBuffers(1, &VBO); //顶点缓冲
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //把顶点数组复制到缓冲中
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //把数据从cpu提取、放入显存的VBO中

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);//顶点
		glEnableVertexAttribArray(0); //启用顶点属性
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));//法线
		glEnableVertexAttribArray(1); //启用顶点属性
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));//纹理
		glEnableVertexAttribArray(2); //启用顶点属性

		glBindVertexArray(0);
	}
};
#endif