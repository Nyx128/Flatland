#include "VMA/vk_mem_alloc.h"
#include "FLVertexBuffer.hpp"
#include "vulkan_asserts.hpp"

FLVertexBuffer::FLVertexBuffer(FLDevice& _device, void* data,VkDeviceSize size):device(_device),
 bufferData(data), memSize(size) {

	VkBuffer stagingBuffer;

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	stagingBufferInfo.size = memSize;

	VmaAllocationCreateInfo stagingBufferAllocInfo{};
	stagingBufferAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	VmaAllocation stagingBufferAllocation;

	auto result = vmaCreateBuffer(device.getAllocator(), &stagingBufferInfo, &stagingBufferAllocInfo, &stagingBuffer, &stagingBufferAllocation, nullptr);
	VK_CHECK_RESULT(result, "Failed to create staging buffer for vertex buffer");


	void* mappedMem;
	vmaMapMemory(device.getAllocator(), stagingBufferAllocation, &mappedMem);
	memcpy(mappedMem, bufferData, memSize);
	vmaUnmapMemory(device.getAllocator(), stagingBufferAllocation);

	VkBufferCreateInfo vertexBufferInfo{};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferInfo.size = memSize;

	//TODO: staging buffers
	VmaAllocationCreateInfo vertexBufferAllocInfo{};
	vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = vmaCreateBuffer(device.getAllocator(), &vertexBufferInfo, &vertexBufferAllocInfo, &vertexBuffer, &vertexBufferAllocation, nullptr);
	VK_CHECK_RESULT(result, "Failed to create vertex buffer");

	copyBuffer(stagingBuffer, vertexBuffer, memSize);

	vmaDestroyBuffer(device.getAllocator(), stagingBuffer, stagingBufferAllocation);

}

FLVertexBuffer::~FLVertexBuffer() {
	vkDeviceWaitIdle(device.getDevice());
	//humble reminder to avoid read access violation :)
	vmaDestroyBuffer(device.getAllocator(), vertexBuffer, vertexBufferAllocation);
	//as stated in their docs https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html
	/*Destroys Vulkan buffer and frees allocated memory.

	This is just a convenience function equivalent to :

	vkDestroyBuffer(device, buffer, allocationCallbacks);
	vmaFreeMemory(allocator, allocation);
	It is safe to pass null as bufferand /or allocation.*/
}

void FLVertexBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.commandBufferCount = 1;
	
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(device.getGraphicsQueue());

	vkFreeCommandBuffers(device.getDevice(), device.getCommandPool(), 1, &commandBuffer);
}
