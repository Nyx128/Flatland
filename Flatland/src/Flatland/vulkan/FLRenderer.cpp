#include <array>
#include <chrono>

#include "FLRenderer.hpp"
#include "../core/logger.hpp"
#include "vulkan_asserts.hpp"
#include "core/FLInputManager.hpp"
#include "FLModel2D.hpp"
#include "ECS/FLComponentManager.hpp"
#include "ECS/components/Renderable.hpp"
#include "ECS/components/Transform2D.hpp"
#include "ECS/components/ImageTexture.hpp"

extern FLComponentManager flComponentManager;

FLRenderer::FLRenderer(FLDevice& _device, FLSwapchain& _swapchain, FLPipeline& _pipeline, std::set<FLEntity>& Entities): device(_device),
swapchain(_swapchain), graphicsPipeline(_pipeline), renderEntities(Entities) {
	FL_TRACE("FLRenderer constructor called");
	createCommandBuffers();
	createSyncObjects();
	buildDescriptorPool();
	allocateDescriptorSets();
	updateDescriptorSets();
}

FLRenderer::~FLRenderer(){
	vkDeviceWaitIdle(device.getDevice());
	FL_TRACE("FLRenderer destructor called");

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
	}
}

void FLRenderer::draw(){
	vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	updateUniformBuffers(currentFrame);

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
	recordCommands(renderEntities, commandBuffers[currentFrame], currentFrame);
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

void FLRenderer::createCommandBuffers(){

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	auto result = vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data());
	VK_CHECK_RESULT(result, "Failed to create renderer command buffers");
}

void FLRenderer::updateUniformBuffers(uint32_t currentImage) {
	GlobalUbo ubo{};
	ubo.color = glm::vec4(0.2, 0, 1.0, 1.0);

	void* data;
	vmaMapMemory(device.getAllocator(), uBuffArr.getAllocation(currentImage), &data);
	memcpy(data, &ubo, sizeof(ubo));
	vmaUnmapMemory(device.getAllocator(), uBuffArr.getAllocation(currentImage));

}

void FLRenderer::recordCommands(std::set<FLEntity> renderEntities, VkCommandBuffer& commandBuffer, uint32_t frameIndex){
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

	FLModel2D::PushConstantData pushData;
	pushData.color = glm::vec3(0.05f, 0.1f, 0.9f);

	for (const FLEntity& entity : renderEntities) {
		auto transform = flComponentManager.GetComponent<Transform2D>(entity);
		auto renderable = flComponentManager.GetComponent<Renderable>(entity);
		pushData.matrix = transform.getTransformMatrix();
		pushData.position = transform.position;
		
		ImageTexture imageTex;

		if (flComponentManager.hasComponent<ImageTexture>(entity)) {
			imageTex = flComponentManager.GetComponent<ImageTexture>(entity);
		}
			
		
		vkCmdPushConstants(commandBuffer, graphicsPipeline.getLayout(),
			VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(FLModel2D::PushConstantData), &pushData);

		VkDeviceSize offsets[] = { 0 };
		VkBuffer vertexBuffers[] = { renderable.model->getVertexBuffer().getBuffer()};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, renderable.model->getIndexBuffer().getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline.getLayout(), 0, 1, &imageTex.descriptorSets[currentFrame], 0, nullptr);
		//draw command
		vkCmdDrawIndexed(commandBuffer, renderable.model->getIndexCount(), 1, 0, 0, 0);
	}
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

void FLRenderer::buildDescriptorPool(){
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;

	descriptorPool.build(poolSizes.data(), poolSizes.size(), MAX_FRAMES_IN_FLIGHT * renderEntities.size());
}

void FLRenderer::allocateDescriptorSets(){
	for (auto& entity : renderEntities) {
		if (flComponentManager.hasComponent<ImageTexture>(entity)) {
			auto imageTex = flComponentManager.GetComponent<ImageTexture>(entity);
			imageTex.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
			descriptorPool.allocateDescriptorSet(MAX_FRAMES_IN_FLIGHT, graphicsPipeline.getDescriptorSetLayout(), imageTex.descriptorSets);
		}
	}
	
}

void FLRenderer::updateDescriptorSets(){
	for (auto& entity : renderEntities) {
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

			if (!flComponentManager.hasComponent<ImageTexture>(entity)) {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uBuffArr.get(i);
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(GlobalUbo);

				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSets[i];
				descriptorWrite.dstBinding = 0;
				descriptorWrite.dstArrayElement = 0;

				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;

				vkUpdateDescriptorSets(device.getDevice(), 1, &descriptorWrite, 0, nullptr);
			}
			else {
				auto imageTex = flComponentManager.GetComponent<ImageTexture>(entity);

				std::array< VkWriteDescriptorSet, 2> descriptorWrite;

				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uBuffArr.get(i);
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(GlobalUbo);

				VkDescriptorImageInfo imageInfo{};
				imageInfo.sampler = imageTex.texture->getSampler();
				imageInfo.imageView = imageTex.texture->getImageView();
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite[0].dstSet = imageTex.descriptorSets[i];
				descriptorWrite[0].dstBinding = 0;
				descriptorWrite[0].dstArrayElement = 0;

				descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite[0].descriptorCount = 1;
				descriptorWrite[0].pBufferInfo = &bufferInfo;

				
				descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite[1].dstSet = imageTex.descriptorSets[i];
				descriptorWrite[1].dstBinding = 1;
				descriptorWrite[1].dstArrayElement = 0;

				descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrite[1].descriptorCount = 1;
				descriptorWrite[1].pImageInfo = &imageInfo;


			}
		}
	}
}

void FLRenderer::updateTextures(VkSampler imageSampler, VkImageView imageView, uint32_t frameIndex) {
	
		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = imageSampler;
		imageInfo.imageView = imageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet descriptorWrite{};	
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[frameIndex];
		descriptorWrite.dstBinding = 1;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;
		

		vkUpdateDescriptorSets(device.getDevice(), 1, &descriptorWrite, 0, nullptr);
	
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
