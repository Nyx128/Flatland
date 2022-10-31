#include "FLUniformBufferArray.hpp"
#include "vulkan_asserts.hpp"

FLUniformBufferArray::FLUniformBufferArray(FLDevice& _device,VkDeviceSize _bufferSize, size_t size)
	:device(_device), arraySize(size),bufferSize(_bufferSize) {
	FL_TRACE("FLUniformBufferArray constructor called");
	createUniformBuffers();
}

FLUniformBufferArray::~FLUniformBufferArray(){
	FL_TRACE("FLUniformBufferArray destructor called");

	for (int i = 0; i < arraySize; i++) {
		vmaDestroyBuffer(device.getAllocator(), uniformBuffers[i], uniformBufferAllocations[i]);
	}
}

void FLUniformBufferArray::createUniformBuffers(){
	uniformBuffers.resize(arraySize);
	uniformBufferAllocations.resize(arraySize);

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.size = bufferSize;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	for (int i = 0; i < arraySize; i++) {
		VK_CHECK_RESULT(vmaCreateBuffer(device.getAllocator(), &bufferInfo, &allocInfo, &uniformBuffers[i], &uniformBufferAllocations[i], nullptr),
			"Failed to create uniform buffer");
	}
}
