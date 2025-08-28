#include "Shader.h"

glm::vec3 Shader::GetPixelColor(const Model& model, glm::vec2 texCoord, glm::vec3 normal)
{
	glm::vec3 color = { 0.0f, 1.0f, 1.0f };

	if (model.HasTexture())
	{
		uint32_t y = (uint32_t)(texCoord.y * (model.m_Texture.Height - 1));
		uint32_t x = (uint32_t)(texCoord.x * (model.m_Texture.Width - 1));

		glm::u8vec4 TextureColor = model.m_Texture.ColorVec[y * model.m_Texture.Width + x];

		color = glm::vec3((float)TextureColor.r, (float)TextureColor.g, (float)TextureColor.b);
	}

	
	if (model.HasNormals())
	{
		float lightIntensity = glm::dot(glm::normalize(normal), LightDir);
		lightIntensity = glm::clamp(lightIntensity, 0.2f, 1.0f);
		color *= lightIntensity;
	}
	
	return color;
}
