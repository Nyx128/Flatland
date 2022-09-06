#pragma once
#include "FLSwapchain.hpp"
#include "FLPipeline.hpp"
#include "FLVertexBuffer.hpp"
#include "ECS/FLEntity.hpp"

#include <set>

class FLRenderer {
public:

	FLRenderer(FLDevice& _device, FLSwapchain& _swapchain, FLPipeline& _pipeline);
	~FLRenderer();

	FLRenderer(const FLRenderer&) = delete;
	FLRenderer& operator=(const FLRenderer&) = delete;

	void draw(std::set<FLEntity> renderEntities);

private:
	const int MAX_FRAMES_IN_FLIGHT = 3;

	FLDevice& device;
	FLSwapchain& swapchain;
	FLPipeline& graphicsPipeline;

	uint32_t imageIndex;
	uint32_t currentFrame = 0;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	void createCommandBuffers();
	void createSyncObjects();
	void beginCommandBuffers(VkCommandBuffer& commandBuffer);
	void beginRenderpass(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
	void recordCommands(std::set<FLEntity> renderEntities, VkCommandBuffer& commandBuffer);
	void endRenderpass(VkCommandBuffer& commandBuffer);
	void endCommandBuffers(VkCommandBuffer& commandBuffer);
};
