#pragma once
#include "FLDevice.hpp"
#include <vector>

class FLDescriptorSetLayout {
public:
	FLDescriptorSetLayout(FLDevice& _device);
	~FLDescriptorSetLayout();

	FLDescriptorSetLayout(const FLDescriptorSetLayout&) = delete;
	FLDescriptorSetLayout& operator=(const FLDescriptorSetLayout&) = delete;

	void AddLayoutBinding(VkDescriptorType type, uint32_t count, VkShaderStageFlagBits stageFlags);
	void build();
	VkDescriptorSetLayout& getLayout() { return descriptorSetLayout; }
private:
	FLDevice& device;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	uint32_t bindingIndex = 0;
	VkDescriptorSetLayout descriptorSetLayout;
	bool built = false;

};