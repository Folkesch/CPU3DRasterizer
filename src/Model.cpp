#include "Model.h"

Model::Model(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec2>& textureCoords, const std::vector<glm::vec3>& normals, const std::vector<TVIndices>& triangleIndices, const std::vector<glm::u8vec3>& texture)
	: ModelTransform(0, 0, { 0, 0, 0 }), Vertices(vertices), TextureCoords(textureCoords), Normals(normals), TriangleIndices(triangleIndices), Texture(texture)
{

	if (Texture.size() == 0)
	{
		TriangleColors.reserve(TriangleIndices.size());
		for (int i = 0; i < TriangleIndices.size(); i++)
		{
			TriangleColors.emplace_back((float)rand() * 255 / RAND_MAX, (float)rand() * 255 / RAND_MAX, (float)rand() * 255 / RAND_MAX);
		}
	}
}

Model::Model(const Model& other)
	: ModelTransform(other.ModelTransform), Vertices(other.Vertices), TextureCoords(other.TextureCoords), Normals(other.Normals), TriangleIndices(other.TriangleIndices), Texture(other.Texture), TriangleColors(other.TriangleColors)
{
}

glm::vec3 Model::GetPixelColor(const glm::vec2& texCoord) const
{

	if (Texture.size() > 0)
	{
		uint32_t texWidthHeight = (uint32_t)glm::sqrt(Texture.size());

		uint32_t idx = (uint32_t)(texCoord.y * texWidthHeight) * texWidthHeight + (uint32_t)(texCoord.x * texWidthHeight);

		glm::uvec3 color = Texture[idx];

		return glm::vec3((float)color.r, (float)color.g, (float)color.b);
	}
	else
	{
		return glm::vec3(0.0f, 1.0f, 1.0f);
	}
}
