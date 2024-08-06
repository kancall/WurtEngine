#include<iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimpToGlm.h"

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp; //ʱ�������¼ʲôʱ�����ʹ�õ�ǰ��ֵ
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		:
		m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f)
	{
		//�Ѷ���һ��ѭ�������й�����λ�á��Ƕȡ���С��Ϣ���洢һ�£����洢һ�����ڶ����е�һ�����ʱ�䣨����ڶ�����ʼʱ��
		m_NumPositions = channel->mNumPositionKeys;
		for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = AssimpToGlm::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
		}
		m_NumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;

			KeyRotation data;
			data.orientation = AssimpToGlm::GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			m_Rotations.push_back(data);
		}
	}
private:
	std::string m_Name; 
	int m_ID;
	glm::mat4 m_LocalTransform;

	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
};