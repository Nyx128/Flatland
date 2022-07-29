#pragma once
#include "FLDevice.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "glm/glm.hpp"

#include <vector>
#include <array>

class FLModel2D {
public:
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDesc{};
			bindingDesc.binding = 0;
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDesc.stride = sizeof(Vertex);
			return bindingDesc;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	FLModel2D(std::vector<Vertex> vertices);
	~FLModel2D();

	FLModel2D(const FLModel2D&) = delete;
	FLModel2D& operator=(const FLModel2D&) = delete;

	std::vector<Vertex> getVertices() { return vertices; }

private:
	std::vector<Vertex> vertices;
};
