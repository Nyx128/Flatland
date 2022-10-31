#pragma once
#include "FLDevice.hpp"
#include "VMA/vk_mem_alloc.h"

class FLVertexBuffer {
public:
	FLVertexBuffer(FLDevice& _device, void* data, VkDeviceSize size);
	~FLVertexBuffer();

	FLVertexBuffer(const FLVertexBuffer&) = delete;
	FLVertexBuffer& operator=(const FLVertexBuffer&) = delete;

	VkBuffer& getBuffer() { return vertexBuffer; }
private:
	VkDeviceSize memSize;
	VkBuffer vertexBuffer;
	VmaAllocation vertexBufferAllocation;

	FLDevice& device;

	void* bufferData;
};
