#pragma once
#include "FLDevice.hpp"
#include <vector>

class FLUniformBufferArray {
public:
	FLUniformBufferArray(FLDevice& _device, VkDeviceSize _bufferSize, size_t arraySize);
	~FLUniformBufferArray();

	FLUniformBufferArray(const FLUniformBufferArray&) = delete;
	FLUniformBufferArray& operator=(const FLUniformBufferArray&) = delete;

	VmaAllocation& getAllocation(size_t index) { return uniformBufferAllocations[index]; }
	VkBuffer& get(size_t index) { return uniformBuffers[index]; }
private:
	FLDevice& device;

	VkDeviceSize bufferSize;
	size_t arraySize;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VmaAllocation> uniformBufferAllocations;

	void createUniformBuffers();
};
