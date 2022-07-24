#include <array>

#include "FLRenderer.hpp"
#include "../core/logger.hpp"
#include "vulkan_asserts.hpp"

FLRenderer::FLRenderer(FLDevice& _device, FLSwapchain& _swapchain, FLPipeline& _pipeline, FLRendererConfig& rendererConfig): device(_device),
swapchain(_swapchain), graphicsPipeline(_pipeline) {
	FL_TRACE("FLRenderer constructor called");
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}

FLRenderer::~FLRenderer(){
	vkDeviceWaitIdle(device.getDevice());
	FL_TRACE("FLRenderer destructor called");
	vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
	}
}

void FLRenderer::draw(){
	vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	auto result = vkAcquireNextImageKHR(device.getDevice(), swapchain.getHandle(), UINT64_MAX,
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain.recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		FL_ASSERT("Failed to acquire image from swapchain");
	}
	vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	beginCommandBuffers(commandBuffers[currentFrame]);
	beginRenderpass(commandBuffers[currentFrame], imageIndex);
	recordCommands(commandBuffers[currentFrame]);
	endRenderpass(commandBuffers[currentFrame]);
	endCommandBuffers(commandBuffers[currentFrame]);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]);
	VK_CHECK_RESULT(result, "Failed to submit draw commands");

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.getHandle();
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||device.getWindow()->isResized()) {
		device.getWindow()->resetResizeFlag();
		swapchain.recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		FL_FATAL("Failed to present swapchain image");
	}
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void FLRenderer::createCommandPool(){
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = device.getGraphicsQueueIndex();

	auto result = vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool);
	VK_CHECK_RESULT(result, "Failed to create renderer command pool");
}

void FLRenderer::createCommandBuffers(){

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	auto result = vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data());
	VK_CHECK_RESULT(result, "Failed to create renderer command buffers");
}

void FLRenderer::recordCommands(VkCommandBuffer& commandBuffer){
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getPipeline());

	//set the dynamic states
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain.getswapchainExtent().width);
	viewport.height = static_cast<float>(swapchain.getswapchainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain.getswapchainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	//draw command
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void FLRenderer::endRenderpass(VkCommandBuffer& commandBuffer){
	vkCmdEndRenderPass(commandBuffer);
}

void FLRenderer::endCommandBuffers(VkCommandBuffer& commandBuffer){
	auto result = vkEndCommandBuffer(commandBuffer);
	VK_CHECK_RESULT(result, "Failed to end command buffer");
}

void FLRenderer::createSyncObjects(){
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		auto sem1_res = vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		auto sem2_res = vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);

		auto fence_result = vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]);

		VK_CHECK_RESULT(sem1_res, "Failed to create image available semaphore");
		VK_CHECK_RESULT(sem2_res, "Failed to create render finished semaphore")
		VK_CHECK_RESULT(fence_result, "Failed to create in flight fence");
	}
}

void FLRenderer::beginRenderpass(VkCommandBuffer& commandBuffer, uint32_t imageIndex){
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = swapchain.getRenderPass();
	renderPassInfo.framebuffer = swapchain.getswapchainFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain.getswapchainExtent();

	std::array<VkClearValue, 2> clearValues;
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };//index 0 color attachment
	clearValues[1].depthStencil = { 1.0f, 0 };//index 1 depth attachment
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void FLRenderer::beginCommandBuffers(VkCommandBuffer& commandBuffer){
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType =VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	auto result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	VK_CHECK_RESULT(result, "Failed to begin command buffer");
}
