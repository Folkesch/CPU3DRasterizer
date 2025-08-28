#include "Transform.h"

Transform::Transform(float yaw, float pitch, glm::vec3 position)
    : Yaw(yaw), Pitch(pitch), Position(position)
{

}

Transform::Transform(const Transform& other)
    : Yaw(other.Yaw), Pitch(other.Pitch), Position(other.Position)
{
}

Transform::~Transform()
{
}

glm::mat3 Transform::GetRotationMat() const
{
    // Yaw Rotation
    glm::mat3 yawMat = {
        glm::cos(Yaw), 0, glm::sin(Yaw),
        0,              1, 0,
        -glm::sin(Yaw),   0, glm::cos(Yaw)
    };

    // Pitch Rotation
    glm::mat3 pitchMat = {
        1, 0, 0,
        0, glm::cos(Pitch), glm::sin(Pitch),
        0, -glm::sin(Pitch), glm::cos(Pitch)
    };

    return pitchMat * yawMat;
}

glm::mat3 Transform::GetInvRotationMat() const
{

    // Revers Yaw Rotation
    glm::mat3 yawMat = {
        glm::cos(-Yaw), 0, glm::sin(-Yaw),
        0,              1, 0,
        -glm::sin(-Yaw),   0, glm::cos(-Yaw)
    };

    // Revers Pitch Rotation
    glm::mat3 pitchMat = {
        1, 0, 0,
        0, glm::cos(-Pitch), glm::sin(-Pitch),
        0, -glm::sin(-Pitch), glm::cos(-Pitch)
    };

    return yawMat * pitchMat;
}

glm::vec3 Transform::ToWorldPoint(glm::vec3 point) const
{
	return point * GetRotationMat() + Position;
}

glm::vec3 Transform::ToLocalPoint(glm::vec3 worldPoint) const
{
    return (worldPoint - Position) * GetInvRotationMat();
}
