#pragma once
#include "FLDevice.hpp"
#include "FLSwapchain.hpp"
#include "FLDescriptorSetLayout.hpp"
#include <array>

class FLPipeline {
public:

	struct FLPipelineConfig {
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		std::array<VkDynamicState, 2> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	};

	struct FLPipelineBuilder {
		FLPipelineConfig pipelineConfig;
		const char* vertPath;
		const char* fragPath;
	};

	FLPipeline(FLDevice& _device, FLSwapchain& _swapchain, FLPipelineBuilder builder);
	~FLPipeline();

	FLPipeline(const FLPipeline&) = delete;
	FLPipeline& operator=(const FLPipeline&) = delete;

	static void createDefaultPipelineConfig(FLPipelineConfig& pipelineConfig);
	VkPipeline& getPipeline() { return graphicsPipeline; }
	VkPipelineLayout getLayout() { return pipelineLayout; }
	std::array<VkDescriptorSetLayout, 3> getUniformBufferLayoutData() {
		std::array<VkDescriptorSetLayout, 3> data;
		auto layout = descriptorSetLayout.getLayout();
		data[0] = layout;
		data[1] = layout;
		data[2] = layout;
		return data;
	}
	VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout.getLayout(); }

private:
	FLDevice& device;
	FLSwapchain& swapchain;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	FLDescriptorSetLayout descriptorSetLayout{device};

	FLPipelineConfig pipelineConfig;
	const char* vertPath;
	const char* fragPath;

	VkShaderModule createShaderModule(std::vector<char>& shaderCode);
	void createGraphicsPipeline();
};
