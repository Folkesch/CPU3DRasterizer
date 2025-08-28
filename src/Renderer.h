#pragma once
#include "Scene.h"
#include "RenderTarget.h"
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() {};
	~Renderer() {};

	void Render(Scene& scene, RenderTarget& renderTarget);

private:
	glm::vec3 WorldToScreen(const glm::vec3& vertex, const Transform& transform, const Camera& Cam, const glm::vec2& numPixels);

	bool PointInTriangel(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p, glm::vec3& weights);

	// Calculates the area of the triangel and returns a negetiv area if the points are in a cc rotation
	float SignedTriangelArea(glm::vec2 a, glm::vec2 b, glm::vec2 p);

	glm::vec2 rotate90DegreesRight(glm::vec2 vec) { return { vec.y, -vec.x }; }
};

