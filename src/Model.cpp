#include "Model.h"

Model::Model(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<TVIndices>& triangleIndices, const Texture& texture)
	: ModelTransform(0, 0, { 0, 0, 0 }), Vertices(vertices), TextureCoords(textureCoords), Normals(normals), TriangleIndices(triangleIndices), m_Texture(texture)
{

	if (m_Texture.ColorVec.size() == 0)
	{
		TriangleColors.reserve(TriangleIndices.size());
		for (int i = 0; i < TriangleIndices.size(); i++)
		{
			TriangleColors.emplace_back((float)rand() * 255 / RAND_MAX, (float)rand() * 255 / RAND_MAX, (float)rand() * 255 / RAND_MAX);
		}
	}
}

Model::Model(const Model& other)
	: ModelTransform(other.ModelTransform), Vertices(other.Vertices), TextureCoords(other.TextureCoords), Normals(other.Normals), TriangleIndices(other.TriangleIndices), m_Texture(other.m_Texture), TriangleColors(other.TriangleColors)
{
}
