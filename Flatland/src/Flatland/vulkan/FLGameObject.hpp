#pragma once
#include <memory>
#include "glm/glm.hpp"

#include "FLModel2D.hpp"
#include "ECS/components/Transform2D.hpp"

using gameObject_ID = std::uint32_t;

class FLGameObject {
public:
	static FLGameObject createGameObject() {
		static gameObject_ID current_id = 0;
		return FLGameObject{ current_id++ };
	}

	FLGameObject(const FLGameObject&) = delete;
	FLGameObject& operator=(const FLGameObject&) = delete;
	FLGameObject(FLGameObject&&) = default;
	FLGameObject& operator=(FLGameObject&&) = default;

	gameObject_ID getID() { return id; }
	std::shared_ptr<FLModel2D> model;

	Transform2D tranform;

private:
	FLGameObject(gameObject_ID _id) : id(_id){}
	gameObject_ID id;
};
