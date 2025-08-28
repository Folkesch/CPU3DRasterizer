#pragma once
#include <glm/glm.hpp>

class Transform
{
public:

	Transform(float yaw, float pitch, glm::vec3 position);

	Transform(const Transform& other);

	~Transform();

	glm::mat3 GetRotationMat() const;
	glm::mat3 GetInvRotationMat() const;

	glm::vec3 ToWorldPoint(glm::vec3 point) const;

	glm::vec3 ToLocalPoint(glm::vec3 worldPoint) const;

	float Yaw;
	float Pitch;
	glm::vec3 Position;
};

