#ifndef MESH_H
#define MESH_H
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <iostream>
#include <string>
#include <vector>

//�洢����
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
//�洢��ͼ
struct Texture
{
	unsigned int id; //����ͼ ��opengl�ж�Ӧ�Ļ�����id
	std::string type; //�������ͣ���ʽΪtexture_xxx
	std::string path;
};
//����
class Mesh
{
public:
	//��������
	std::vector<Vertex> vertices; //����
	std::vector<unsigned int> indices; //��������
	std::vector<Texture> textures; //����
	unsigned int VAO;

	//��ʼ����������
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
			glActiveTexture(GL_TEXTURE0 + i); //��������

			std::string number;
			std::string name = textures[i].type;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++);

			glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		//��������
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0); //����
		glBindVertexArray(0);

		//glActiveTexture(GL_TEXTURE0);
	}

	bool pickTrace(glm::vec3 direc, glm::vec3 cameraPos, glm::vec3 modelWorldPos)
	{
		//��ƬԪ�ж���û�н��㣬����з���true
		for (int i = 0; i < indices.size(); i+=3)
		{
			glm::vec3 v1 = vertices[indices[i]].Position + modelWorldPos;
			glm::vec3 v2 = vertices[indices[i + 1]].Position + modelWorldPos;
			glm::vec3 v3 = vertices[indices[i + 2]].Position + modelWorldPos;

			glm::vec3 dir1 = v2 - v1;
			glm::vec3 dir2 = v3 - v1;
			glm::vec3 n = glm::normalize(glm::cross(dir1, dir2));

			//���ж����ߺ����������������޽���
			glm::vec3 eyeview = cameraPos - direc;
			double t = glm::dot(v1 - cameraPos, n) / glm::dot(n, eyeview);
			if (t >= 0)
				continue;
			//���жϸý����Ƿ����������ڲ�
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
	//��������
	unsigned int VBO, EBO;

	//�󶨶�������
	void setUpMesh()
	{
		glGenVertexArrays(1, &VAO);//��������
		glGenBuffers(1, &VBO); //���㻺��
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //�Ѷ������鸴�Ƶ�������
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //�����ݴ�cpu��ȡ�������Դ��VBO��

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);//����
		glEnableVertexAttribArray(0); //���ö�������
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));//����
		glEnableVertexAttribArray(1); //���ö�������
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));//����
		glEnableVertexAttribArray(2); //���ö�������

		glBindVertexArray(0);
	}
};
#endif