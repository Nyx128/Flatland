#include "FLDescriptorPool.hpp"
#include "vulkan_asserts.hpp"

FLDescriptorPool::FLDescriptorPool(FLDevice& _device):device(_device) {
	FL_TRACE("FLDescriptorPool constructor called");
	
}

FLDescriptorPool::~FLDescriptorPool() {
	FL_ASSERT_MSG(built, "can't destroy descriptor pool without calling build()");

	vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
}

void FLDescriptorPool::build(VkDescriptorPoolSize* poolSizes, uint32_t poolSizeCount, uint32_t maxSets){
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizeCount;
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = maxSets;

	VK_CHECK_RESULT(vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool),
		"Failed to create Descriptor pool");
	built = true;
}

void FLDescriptorPool::allocateDescriptorSet(uint32_t count, VkDescriptorSetLayout& layout,
	std::vector<VkDescriptorSet>& descriptorSets) {
	std::vector<VkDescriptorSetLayout> layouts(count, layout);
	FL_ASSERT_MSG(built, "can't allocate descriptor sets from pool without calling build()");
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = count;
	allocInfo.pSetLayouts = layouts.data();

	VK_CHECK_RESULT(vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()),
		"Failed to allocate descriptor sets");
}