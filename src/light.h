#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class DirLight
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	DirLight()
	{
		position = glm::vec3(1.0, 1.0, 1.0);
		direction = glm::vec3(-0.2f, -1.0f, -0.3f);

		ambient = glm::vec3(0.2f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(1.0f);
	}
	~DirLight() {};

private:
};

class PointLight
{
public:
	glm::vec3 position;

	float constant; //点光源的常数项
	float linear; //点光源的一次项
	float quadratic; //点光源的二次项

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	PointLight()
	{
		position = glm::vec3(1.0f);

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;

		ambient = glm::vec3(0.2f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(1.0f);
	}
	~PointLight() {};
private:
};

class SpotLight
{
public:
	SpotLight()
	{
		position = glm::vec3(1.0f);
		direction = glm::vec3(0.0, 0.0, -1.0);

		cutOff = glm::cos(glm::radians(12.5f));
		outerCutOff = glm::cos(glm::radians(17.5f));

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;

		ambient = glm::vec3(0.2f);
		diffuse = glm::vec3(0.5f);
		specular = glm::vec3(1.0f);
	}
	~SpotLight() {};

private:
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff; //切光角
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

#endif // !LIGHT_H
