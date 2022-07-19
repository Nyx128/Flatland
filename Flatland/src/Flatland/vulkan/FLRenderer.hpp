#pragma once
#include "FLSwapchain.hpp"
#include "FLPipeline.hpp"

class FLRenderer {
public:

	struct FLRendererConfig {
	};

	FLRenderer(FLDevice& _device, FLSwapchain& _swapchain, FLPipeline& _pipeline, FLRendererConfig& rendererConfig);
	~FLRenderer();

	FLRenderer(const FLRenderer&) = delete;
	FLRenderer& operator=(const FLRenderer&) = delete;

	void draw();

private:
	FLDevice& device;
	FLSwapchain& swapchain;
	FLPipeline& graphicsPipeline;

	uint32_t imageIndex;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void createCommandPool();
	void createCommandBuffers();
	void createSyncObjects();
	void beginCommandBuffers(VkCommandBuffer& commandBuffer);
	void beginRenderpass(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
	void recordCommands(VkCommandBuffer& commandBuffer);
	void endRenderpass(VkCommandBuffer& commandBuffer);
	void endCommandBuffers(VkCommandBuffer& commandBuffer);
};
