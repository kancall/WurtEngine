#ifndef EDITOR_DATA_H
#define EDITOR_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "light.h"

#include <iostream>
#include <vector>

class EditorData
{
public:
	//light
	int dirLightCount; //平行光源的数量
	int pointLightCount;
	int spotLightCount;

	std::vector<DirLight> dirLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	EditorData() 
	{
		//light
		dirLightCount = 0;
		pointLightCount = 1;
		spotLightCount = 0;

		dirLights.resize(dirLightCount);
		pointLights.resize(pointLightCount);
		spotLights.resize(spotLightCount);
	}
	~EditorData() {};

private:

};

#endif // !EDITOR_DATA_H
