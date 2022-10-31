#pragma once
#include "../ECS/FLSystem.hpp"
#include "../ECS/FLComponentManager.hpp"
#include "../ECS/FLEntityManager.hpp"
#include "../ECS/FLSystemManager.hpp"
#include "glm/glm.hpp"

class CircularMotion : FLSystem {
public:
	CircularMotion();
	~CircularMotion();

	void update(float dt);
	static void initSystem(FLComponentManager& componentManager, FLEntityManager& entityManager, FLSystemManager& systemManager);
private:
	float radius = 0.0f;
	float mass = 0.0f;
	float a_vel = 0.0f;

	glm::vec2 centre = glm::vec2(0.0f);
};
