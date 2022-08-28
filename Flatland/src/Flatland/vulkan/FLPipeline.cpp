#include "FLPipeline.hpp"
#include "FLUtil.hpp"
#include "vulkan_asserts.hpp"
#include "FLModel2D.hpp"

#include <iostream>

FLPipeline::FLPipeline(FLDevice& _device, FLSwapchain& _swapchain, FLPipelineBuilder builder):device(_device), swapchain(_swapchain) {
	FL_TRACE("FLPipeline constructor called");
	pipelineConfig = builder.pipelineConfig;
	vertPath = builder.vertPath;
	fragPath = builder.fragPath;

	createGraphicsPipeline();
}

FLPipeline::~FLPipeline(){
	FL_TRACE("FLPipeline destructor called");
	vkDestroyPipelineLayout(device.getDevice(), pipelineLayout, nullptr);
	vkDestroyPipeline(device.getDevice(), graphicsPipeline, nullptr);
}

VkShaderModule FLPipeline::createShaderModule(std::vector<char>& shaderCode) {
	VkShaderModuleCreateInfo moduleInfo{};
	moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleInfo.codeSize = static_cast<uint32_t>(shaderCode.size());
	moduleInfo.pCode = reinterpret_cast<uint32_t*>(shaderCode.data());
	
	VkShaderModule shaderModule;
	auto result = vkCreateShaderModule(device.getDevice(), &moduleInfo, nullptr, &shaderModule);
	VK_CHECK_RESULT(result, "Failed to created shader module");

	return shaderModule;
}

void FLPipeline::createDefaultPipelineConfig(FLPipelineConfig& pipelineConfig){
	pipelineConfig.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineConfig.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	pipelineConfig.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipelineConfig.viewportInfo.pNext = nullptr;
	pipelineConfig.viewportInfo.viewportCount = 1;
	pipelineConfig.viewportInfo.pViewports = nullptr;
	pipelineConfig.viewportInfo.scissorCount = 1;
	pipelineConfig.viewportInfo.pScissors = nullptr;

	pipelineConfig.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineConfig.rasterizationInfo.depthClampEnable = VK_FALSE;
	pipelineConfig.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineConfig.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
	pipelineConfig.rasterizationInfo.lineWidth = 1.0f;
	pipelineConfig.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineConfig.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	pipelineConfig.rasterizationInfo.depthBiasEnable = VK_FALSE;
	pipelineConfig.rasterizationInfo.depthBiasConstantFactor = 0.0f; 
	pipelineConfig.rasterizationInfo.depthBiasClamp = 0.0f;          
	pipelineConfig.rasterizationInfo.depthBiasSlopeFactor = 0.0f;    

	pipelineConfig.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipelineConfig.multisampleInfo.sampleShadingEnable = VK_FALSE;
	pipelineConfig.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineConfig.multisampleInfo.minSampleShading = 1.0f;           
	pipelineConfig.multisampleInfo.pSampleMask = nullptr;             
	pipelineConfig.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  
	pipelineConfig.multisampleInfo.alphaToOneEnable = VK_FALSE;       

	pipelineConfig.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	pipelineConfig.colorBlendAttachment.blendEnable = VK_TRUE;
	pipelineConfig.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	pipelineConfig.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	pipelineConfig.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	pipelineConfig.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	pipelineConfig.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	pipelineConfig.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;      

	pipelineConfig.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipelineConfig.colorBlendInfo.logicOpEnable = VK_FALSE;
	pipelineConfig.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY; 
	pipelineConfig.colorBlendInfo.attachmentCount = 1;
	pipelineConfig.colorBlendInfo.pAttachments = &pipelineConfig.colorBlendAttachment;
	pipelineConfig.colorBlendInfo.blendConstants[0] = 0.0f; 
	pipelineConfig.colorBlendInfo.blendConstants[1] = 0.0f; 
	pipelineConfig.colorBlendInfo.blendConstants[2] = 0.0f; 
	pipelineConfig.colorBlendInfo.blendConstants[3] = 0.0f; 

	pipelineConfig.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipelineConfig.depthStencilInfo.depthTestEnable = VK_TRUE;
	pipelineConfig.depthStencilInfo.depthWriteEnable = VK_TRUE;
	pipelineConfig.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	pipelineConfig.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	pipelineConfig.depthStencilInfo.minDepthBounds = 0.0f;  
	pipelineConfig.depthStencilInfo.maxDepthBounds = 1.0f;  
	pipelineConfig.depthStencilInfo.stencilTestEnable = VK_FALSE;
	pipelineConfig.depthStencilInfo.front = {};  
	pipelineConfig.depthStencilInfo.back = {};   

	pipelineConfig.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	pipelineConfig.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipelineConfig.dynamicStateInfo.pDynamicStates = pipelineConfig.dynamicStateEnables.data();
	pipelineConfig.dynamicStateInfo.dynamicStateCount =
		static_cast<uint32_t>(pipelineConfig.dynamicStateEnables.size());
	pipelineConfig.dynamicStateInfo.flags = 0;
}

void FLPipeline::createGraphicsPipeline(){
	//create shader module
	std::vector<char> vertCode = FLUtil::readFile(vertPath);
	std::vector<char> fragCode = FLUtil::readFile(fragPath);

	VkShaderModule vertShaderModule = createShaderModule(vertCode);
	VkShaderModule fragShaderModule = createShaderModule(fragCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	//

	//create Pipeline layout

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.size = sizeof(FLModel2D::PushConstantData);
	pushConstantRange.offset = 0;
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	pipelineLayoutInfo.pushConstantRangeCount = 1;

	auto result = vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
	VK_CHECK_RESULT(result, "Failed to create Pipeline layout");
	//

	auto VertexBindingDescriptions = FLModel2D::Vertex::getBindingDescription();
	auto VertexAttributeDescriptions = FLModel2D::Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &VertexBindingDescriptions;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(VertexAttributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = VertexAttributeDescriptions.data();

	//create graphics pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &pipelineConfig.inputAssemblyInfo;
	pipelineInfo.pViewportState = &pipelineConfig.viewportInfo;
	pipelineInfo.pRasterizationState = &pipelineConfig.rasterizationInfo;
	pipelineInfo.pMultisampleState = &pipelineConfig.multisampleInfo;
	pipelineInfo.pColorBlendState = &pipelineConfig.colorBlendInfo;
	pipelineInfo.pDepthStencilState = &pipelineConfig.depthStencilInfo;
	pipelineInfo.pDynamicState = &pipelineConfig.dynamicStateInfo;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = swapchain.getRenderPass();
	pipelineInfo.subpass = 0;

	result = vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	VK_CHECK_RESULT(result, "Failed to create graphics pipeline");
	
	vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
	//
}
