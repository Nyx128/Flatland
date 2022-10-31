#include "FLDescriptorSetLayout.hpp"
#include "vulkan_asserts.hpp"

FLDescriptorSetLayout::FLDescriptorSetLayout(FLDevice& _device):device(_device){
    FL_TRACE("FLDescriptorSetLayout created");
}

FLDescriptorSetLayout::~FLDescriptorSetLayout(){
    FL_TRACE("FLDescriptorSetLayout destroyed");
    FL_ASSERT(built, "The descriptorSetLayout can't be destroyed without calling build()");
    vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
}

void FLDescriptorSetLayout::AddLayoutBinding(VkDescriptorType type, uint32_t count, VkShaderStageFlagBits stageFlags){
    FL_ASSERT_MSG(!built, "Cannot add layout binding after building the descriptor set");
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = bindingIndex;
    uboLayoutBinding.descriptorType = type;
    uboLayoutBinding.descriptorCount = count;
    uboLayoutBinding.stageFlags = stageFlags;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    layoutBindings.push_back(std::move(uboLayoutBinding));
    bindingIndex++;
}

void FLDescriptorSetLayout::build(){
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout),
        "Failed to create descriptor set layout");

    built = true;
}
