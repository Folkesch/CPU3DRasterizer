#pragma once
#include <vector>

#include <glm/glm.hpp>
#include "Transform.h"

#include <stdlib.h>     /* srand, rand */

struct TVIndices
{
	int32_t vertexIdx;
	int32_t textureCoordIdx;
	int32_t normalIdx;
};

class Texture
{
public:
	Texture(uint32_t width, uint32_t height, std::vector<glm::u8vec4> Data)
		: Width(width), Height(height), Data(Data)
	{
	}

	~Texture() {}


	uint32_t Width;
	uint32_t Height;
	std::vector<glm::u8vec4> Data; // RGBA
};

class Model
{
public:
	Model(const std::vector<glm::vec3>& vertices,
		const std::vector<glm::vec2>& textureCoords,
		const std::vector<glm::vec3>& normals,
		const std::vector<TVIndices>& triangleIndices,
		const std::vector<glm::u8vec3>& texture);

	Model(const Model& copy);

	glm::vec3 GetPixelColor(const glm::vec2& texCoord) const;

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec2> TextureCoords;
	std::vector<glm::vec3> Normals;

	std::vector<TVIndices> TriangleIndices;

	std::vector<glm::vec3> TriangleColors;
	std::vector<glm::u8vec3> Texture;

	Transform ModelTransform;
};

