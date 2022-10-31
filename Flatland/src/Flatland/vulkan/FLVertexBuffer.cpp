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

	VmaAllocationCreateInfo vertexBufferAllocInfo{};
	vertexBufferAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	result = vmaCreateBuffer(device.getAllocator(), &vertexBufferInfo, &vertexBufferAllocInfo, &vertexBuffer, &vertexBufferAllocation, nullptr);
	VK_CHECK_RESULT(result, "Failed to create vertex buffer");

	device.copyBuffer(stagingBuffer, vertexBuffer, memSize);

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
 
