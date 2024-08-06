#ifndef MODEL_H
#define MODEL_H
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include "shader.h"
#include "mesh.h"

#include <iostream>
#include <vector>
#include <map>

//骨骼数据
struct BoneInfo
{
	int id; //骨骼id
	glm::mat4 offset; //模型空间中，骨骼相对于模型中心的偏移矩阵
};

class Model
{
public:
	static unsigned int cnt; //用于计数
	unsigned int ID; //模型特有的ID，后续使用这个独有的ID对模型进行查找
	std::string name; //模型的名字

	glm::vec3 position;
	glm::vec3 rotation;
	//遇到了一个bug，就是当改变模型的scale的时候，如果把模型变小，但是在使用射线检测的时候，实际上判断的位置还是原来没变小的模型，待修改
	glm::vec3 scale;
	//模型数据
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	Model(std::string const& path)
	{
		ID = cnt++;
		name = path; //测试用，后续更改
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);

		//std::cout << "Model::Model id = " << ID << std::endl;

		loadModel(path);
	}

	void Draw(Shader* shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader);
	}
	bool pickTrace(glm::vec3 direc, glm::vec3 cameraPos)
	{
		//遍历所有mesh判断有没有交点，如果有返回true
		for (int i = 0; i < meshes.size(); i++)
		{
			if (meshes[i].pickTrace(direc, cameraPos, this->position))
			{
				return true;
			}
		}
		return false;
	}
private:
	std::map<std::string, BoneInfo> m_BoneInfoMap; //<骨骼名称，骨骼信息>
	int m_BoneCounter = 0; //骨骼数量的计数器

	void loadModel(std::string const& path) //path:模型的路径
	{
		//加载模型
		Assimp::Importer import;
		const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		//对模型中的每个节点进行处理
		processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode* node, const aiScene* scene)
	{
		// 处理节点所有的网格
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}
		// 接下来对它的子节点重复这一过程
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}
	//给骨骼相关的值进行初始化
	void setVertexBoneDataToDefault(Vertex& vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.m_BoneIDs[i] = -1;
			vertex.m_weights[i] = 0.0f;
		}
	}
	//填充顶点的骨骼相关信息
	void setBertexBoneData(Vertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			if (vertex.m_BoneIDs[i] < 0)
			{
				vertex.m_weights[i] = weight;
				vertex.m_BoneIDs[i] = boneID;
				break;
			}
		}
	}
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;

		//处理顶点数据
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			setVertexBoneDataToDefault(vertex); //对与这个顶点相关的骨骼们进行初始化
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			//这里只用顶点的第一组纹理坐标
			if (mesh->mTextureCoords[0])
			{
				vector.x = mesh->mTextureCoords[0][i].x;
				vector.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vector;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		//处理网格索引
		//按照网格的三角形顺序遍历
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int t = 0; t < face.mNumIndices; t++)
				indices.push_back(face.mIndices[t]);
		}
		//处理贴图材质
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			//如果没有贴图，添加一个默认的diffuse贴图
			if (textures.size() <= 0)
			{
				Texture texture;
				texture.id = TextureFromFile("/floor.jpg", "E://vs c++ practice//WurtEngine//WurtEngine//res//texture/");
				texture.type = "texture_diffuse";
				texture.path = "E://vs c++ practice//WurtEngine//WurtEngine//res//texture//floor.jpg";
				textures.push_back(texture);
			}
		}
		//获取骨骼实际的权重
		ExtractBoneWeightForVertices(vertices, mesh, scene);

		//如果模型的scale更改了，那么每个片元的点也应该跟着一起改，在这里记录一下，后续修改
		return Mesh(vertices, indices, textures); 
	}
	//获得骨骼实际的权重数据
	//aiBone是存储在aiMesh中的，所以要在mesh中去获取详细的骨骼信息，具体的关系可以看learnOpengl上的关系图
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) //骨骼没存过
			{
				BoneInfo nowBoneInfo;

				nowBoneInfo.id = m_BoneCounter;
				aiMatrix4x4 from = mesh->mBones[boneIndex]->mOffsetMatrix; //提取骨骼的偏移矩阵
				glm::mat4 to;
				to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
				to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
				to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
				to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
				nowBoneInfo.offset = to;

				m_BoneInfoMap[boneName] = nowBoneInfo;
				boneID = m_BoneCounter;
				m_BoneCounter++;
			}
			else
			{
				boneID = m_BoneInfoMap[boneName].id;
			}
			assert(boneID != -1);

			auto weights = mesh->mBones[boneIndex]->mWeights;//当前骨骼对应的权重值
			for (int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; weightIndex++)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				setBertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{   // 如果纹理还没有被加载，则加载它
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // 添加到已加载的纹理中
			}
		}
		return textures;
	}
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
};

#endif
