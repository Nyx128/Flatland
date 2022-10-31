#pragma once
#include "FLDevice.hpp"
#include "VMA/vk_mem_alloc.h"

class FLIndexBuffer {
public:
	FLIndexBuffer(FLDevice& _device, void* data, VkDeviceSize size);
	~FLIndexBuffer();

	FLIndexBuffer(const FLIndexBuffer&) = delete;
	FLIndexBuffer& operator=(const FLIndexBuffer&) = delete;

	VkBuffer& getBuffer() { return indexBuffer; }
private:
	VkDeviceSize memSize;
	VkBuffer indexBuffer;
	VmaAllocation indexBufferAllocation;

	FLDevice& device;

	void* bufferData;
};
