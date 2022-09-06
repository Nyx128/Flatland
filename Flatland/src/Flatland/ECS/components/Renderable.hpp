#pragma once
#include "FLModel2D.hpp"
#include <memory>

struct Renderable {
	std::shared_ptr<FLModel2D> model;
};