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
	Texture(uint32_t width, uint32_t height, std::vector<glm::u8vec4> ColorVec)
		: Width(width), Height(height), ColorVec(ColorVec)
	{
	}

	Texture(const Texture& copy)
		: Width(copy.Width), Height(copy.Height), ColorVec(copy.ColorVec)
	{
	}

	~Texture() {}

	uint32_t Width;
	uint32_t Height;
	std::vector<glm::u8vec4> ColorVec; // RGBA
};

class Model
{
public:
	Model(const std::vector<glm::vec3>& vertices,
		const std::vector<glm::vec2>& textureCoords,
		const std::vector<glm::vec3>& normals,
		const std::vector<TVIndices>& triangleIndices,
		const Texture& texture);

	Model(const Model& copy);

	bool HasTexture() const { return TextureCoords.size() > 0 && m_Texture.ColorVec.size() > 0; }
	bool HasNormals() const { return Normals.size() > 0; }

	Transform ModelTransform;

	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec2> TextureCoords;
	std::vector<glm::vec3> Normals;

	std::vector<TVIndices> TriangleIndices;

	std::vector<glm::vec3> TriangleColors;
	Texture m_Texture;
private:
};

