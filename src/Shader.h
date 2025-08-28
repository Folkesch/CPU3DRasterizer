#pragma once
#include <glm/glm.hpp>

#include "Model.h"

class Shader
{
public:
	Shader(glm::vec3 lightDir)
		: LightDir(glm::normalize(lightDir))
	{}
	~Shader() {}

	glm::vec3 GetPixelColor(const Model& model, glm::vec2 texCoord, glm::vec3 normal);

	glm::vec3 LightDir;
};