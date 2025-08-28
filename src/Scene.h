#pragma once
#include <vector>
#include <string>

#include <stdlib.h>     // srand 
#include <time.h>       // time 

#include "Model.h"
#include "Transform.h"
#include "Shader.h"

class Camera
{
public:
	
	Camera()
		: CameraTransform(0, 0, { 0, 0, 0 }) {};
	~Camera() {};

	void MoveForward(float movementAmount);

	void MoveRight(float movementAmount);

	void MoveUp(float movementAmount);

	float Fov = 60.0f; // degrees
	Transform CameraTransform;
private:

};

class Scene
{
public:

	Scene()
		: Cam(), Shader({ 1, 2, -3 })
	{ srand((uint32_t)time(NULL)); };
	~Scene() {};


	// TODO 
	Scene(const Scene& scene) = delete;

	void Update(float deltaTime);

	int32_t LoadModelObj(std::string objFilePath);


	std::vector<Model> Models;
	Camera Cam;
	Shader Shader;
};