#pragma once
#include <vector>
#include "FLDevice.hpp"

class FLDescriptorPool {
public:
	FLDescriptorPool(FLDevice& _device);
	~FLDescriptorPool();

	void build(VkDescriptorPoolSize* poolSizes, uint32_t poolSizeCount, uint32_t maxSets);
	bool built = false;

	FLDescriptorPool(const FLDescriptorPool&) = delete;
	FLDescriptorPool& operator=(const FLDescriptorPool&) = delete;

	void allocateDescriptorSet(uint32_t count, VkDescriptorSetLayout& layout , std::vector<VkDescriptorSet>& descriptorSets);
private:
	FLDevice& device;
	VkDescriptorPool descriptorPool;
};
