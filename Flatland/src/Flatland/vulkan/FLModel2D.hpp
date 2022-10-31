#pragma once
#include "FLDevice.hpp"
#include "FLVertexBuffer.hpp"
#include "FLIndexBuffer.hpp"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif 

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#ifndef GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_LEFT_HANDED
#endif 


#include "glm/glm.hpp"

#include <vector>
#include <array>

class FLModel2D {
public:
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDesc{};
			bindingDesc.binding = 0;
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDesc.stride = sizeof(Vertex);
			return bindingDesc;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};

	struct PushConstantData {
		glm::mat2 matrix{1.0f};
		glm::vec2 position;
		alignas(16)glm::vec3 color;
	};

	FLModel2D(FLDevice& device, std::vector<Vertex> vertices, std::vector<uint32_t> indices);
	~FLModel2D();

	FLModel2D(const FLModel2D&) = delete;
	FLModel2D& operator=(const FLModel2D&) = delete;

	FLVertexBuffer& getVertexBuffer() { return vertexBuffer; }
	FLIndexBuffer& getIndexBuffer() { return indexBuffer; }
	uint32_t getVertexCount() const { return static_cast<uint32_t>(vertices.size()); }
	uint32_t getIndexCount() const { return indexCount; }
	std::vector<Vertex> getVertices() { return vertices; }

	static std::shared_ptr<FLModel2D> createCircle(FLDevice& device, uint32_t numSides, glm::vec3 color);
	static std::shared_ptr<FLModel2D> createSquare(FLDevice& device, glm::vec2 offset, glm::vec3 color);
private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t indexCount = static_cast<uint32_t>(indices.size());
	FLDevice& device;

	FLVertexBuffer vertexBuffer{device, vertices.data(), (VkDeviceSize)sizeof(Vertex) * vertices.size()};
	FLIndexBuffer indexBuffer{ device, indices.data(), (VkDeviceSize)sizeof(uint32_t) * indices.size() };
};
