#pragma once
#include "FLSwapchain.hpp"
#include "FLPipeline.hpp"
#include "FLVertexBuffer.hpp"
#include "ECS/FLEntity.hpp"
#include "FLUniformBufferArray.hpp"
#include "globalUbo.hpp"
#include "FLDescriptorPool.hpp"

#include <set>

class FLRenderer {
public:

	FLRenderer(FLDevice& _device, FLSwapchain& _swapchain, FLPipeline& _pipeline, std::set<FLEntity>& Entities);
	~FLRenderer();

	FLRenderer(const FLRenderer&) = delete;
	FLRenderer& operator=(const FLRenderer&) = delete;

	void draw();

private:
	const int MAX_FRAMES_IN_FLIGHT = 3;

	FLDevice& device;
	FLSwapchain& swapchain;
	FLPipeline& graphicsPipeline;

	uint32_t imageIndex;
	uint32_t currentFrame = 0;

	std::set<FLEntity>& renderEntities;

	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	FLUniformBufferArray uBuffArr{ device, sizeof(GlobalUbo), MAX_FRAMES_IN_FLIGHT };
	FLDescriptorPool descriptorPool{ device };
	std::vector<VkDescriptorSet> descriptorSets;

	void createCommandBuffers();
	void createSyncObjects();
	void buildDescriptorPool();
	void allocateDescriptorSets();
	void updateDescriptorSets();
	void updateTextures(VkSampler imageSampler, VkImageView imageView, uint32_t frameIndex);
	void beginCommandBuffers(VkCommandBuffer& commandBuffer);
	void beginRenderpass(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
	void recordCommands(std::set<FLEntity> renderEntities, VkCommandBuffer& commandBuffer, uint32_t frameIndex);
	void updateUniformBuffers(uint32_t currentImage);
	void endRenderpass(VkCommandBuffer& commandBuffer);
	void endCommandBuffers(VkCommandBuffer& commandBuffer);
};
