#include "FLModel2D.hpp"
#include "glm/gtc/constants.hpp"

FLModel2D::FLModel2D(FLDevice& device, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	: device(device), vertices(vertices), indices(indices) {

}

FLModel2D::~FLModel2D(){

}

std::shared_ptr<FLModel2D> FLModel2D::createCircle(FLDevice& device, uint32_t numSides, glm::vec3 color){
	std::vector<Vertex> vertices;
	float radPerRot = glm::pi<float>() * 2.0f/numSides;

	vertices.emplace_back(Vertex{glm::vec2(0.0f, 0.0f), color });

	for (int i = 0; i < numSides; i++) {
		float angle = radPerRot * i;
		vertices.emplace_back(Vertex{ glm::vec2(cos(angle), sin(angle)), color });
	}

	std::vector<uint32_t> indices;

	for (int i = 0; i < numSides; i++) {
		indices.emplace_back(0);
		indices.emplace_back(i + 1);
		indices.emplace_back(((i + 2) > numSides)? (i+2)- numSides : i + 2);
	}

	std::shared_ptr<FLModel2D> circle = std::make_shared<FLModel2D>(device, vertices, indices);
	return circle;
}

std::shared_ptr<FLModel2D> FLModel2D::createSquare(FLDevice& device, glm::vec2 offset, glm::vec3 color){
	std::vector<Vertex> vertices = {
	{glm::vec2(- 0.5f, -0.5f) + offset, color, {1.0f, 0.0f}},
	{glm::vec2(0.5f, -0.5f) + offset, color, {0.0f, 0.0f}},
	{ glm::vec2(0.5f, 0.5f) + offset, color, {0.0f, 1.0f}},
	{ glm::vec2(-0.5f, 0.5f) + offset, color, {1.0f, 1.0f}}
	};

	std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0
	};

	std::shared_ptr<FLModel2D> quad = std::make_shared<FLModel2D>(device, vertices, indices);
	return quad;
}
