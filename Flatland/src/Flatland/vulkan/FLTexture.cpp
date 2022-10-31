#define STB_IMAGE_IMPLEMENTATION
#include "FLTexture.hpp"
#include "core/asserts.hpp"
#include "vulkan_asserts.hpp"
#include "core/logger.hpp"
#include <stdexcept>



FLTexture::FLTexture(FLDevice& _device, const char* filepath):device(_device), filepath(filepath) {
	getImage(filepath);
	FL_TRACE("FLTexture constructed using %s", filepath);
	createImage();
	createImageView();
	createSampler();
}

FLTexture::~FLTexture(){
	vkDeviceWaitIdle(device.getDevice());
	FL_TRACE("destructor called for FLTexture using %s", filepath);
	vkDestroySampler(device.getDevice(), imageSampler, nullptr);
	vkDestroyImageView(device.getDevice(), imageView, nullptr);
	vmaDestroyImage(device.getAllocator(), textureImage, textureImageAlloc);
}

void FLTexture::getImage(const char* filepath) {
	
	//first lets get the image data into "pixels" using stbi_load
	pixels = stbi_load(filepath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	imageSize = texWidth * texHeight * texChannels *  sizeof(stbi_uc);

	FL_ASSERT_MSG(pixels, "Failed to load image");
}

void FLTexture::createImage(){
	//now lets copy over the image data we loaded into a staging buffer 
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	VkBufferCreateInfo stagingBufferInfo{};
	stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	stagingBufferInfo.size = imageSize;

	VmaAllocationCreateInfo stagingAlloc{};
	stagingAlloc.usage = VMA_MEMORY_USAGE_CPU_ONLY;

	VmaAllocation stagingBufferAllocation;

	auto result = vmaCreateBuffer(device.getAllocator(), &stagingBufferInfo, &stagingAlloc, &stagingBuffer, &stagingBufferAllocation, nullptr);
	VK_CHECK_RESULT(result, "Failed to create staging buffer for vertex buffer");

	void* data;
	vmaMapMemory(device.getAllocator(), stagingBufferAllocation, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(device.getAllocator(), stagingBufferAllocation);

	stbi_image_free(pixels);

	//now create the image
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = static_cast<uint32_t>(texWidth);
	imageInfo.extent.height = static_cast<uint32_t>(texHeight);
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	VmaAllocationCreateInfo imageAllocCreateInfo{};
	imageAllocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VK_CHECK_RESULT(vmaCreateImage(device.getAllocator(), &imageInfo, &imageAllocCreateInfo, &textureImage, &textureImageAlloc, nullptr),
		"Failed to create image");
	//our image we just created has undefined layout so lets transition it to a layout suitable for transfer_dst
	//(optimal when we wanna copy something to the image)
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	//the staging buffer can now be copied into the image, now it has the image data we supplied
	copyBufferToImg(stagingBuffer, textureImage, texWidth, texHeight);

	//before using this image into the shader, we will use another pipeline barrier to make it shader readable
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vmaDestroyBuffer(device.getAllocator(), stagingBuffer, stagingBufferAllocation);
}

void FLTexture::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout){
	VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

	VkImageMemoryBarrier memBarrier{};
	memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	memBarrier.oldLayout = oldLayout;
	memBarrier.newLayout = newLayout;
	memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.image = image;
	memBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	memBarrier.subresourceRange.baseMipLevel = 0;
	memBarrier.subresourceRange.levelCount = 1;
	memBarrier.subresourceRange.baseArrayLayer = 0;
	memBarrier.subresourceRange.layerCount = 1;
	memBarrier.srcAccessMask = 0;
	memBarrier.dstAccessMask = 0;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		memBarrier.srcAccessMask = 0;
		memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	//for convinience
	/*void vkCmdPipelineBarrier(
		VkCommandBuffer                             commandBuffer,
		VkPipelineStageFlags                        srcStageMask,
		VkPipelineStageFlags                        dstStageMask,
		VkDependencyFlags                           dependencyFlags,
		uint32_t                                    memoryBarrierCount,
		const VkMemoryBarrier*						pMemoryBarriers,
		uint32_t                                    bufferMemoryBarrierCount,
		const VkBufferMemoryBarrier*				pBufferMemoryBarriers,
		uint32_t                                    imageMemoryBarrierCount,
		const VkImageMemoryBarrier*					pImageMemoryBarriers);*/

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &memBarrier
	);

	device.endSingleTimeCommands(commandBuffer);
}

void FLTexture::copyBufferToImg(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height){
	VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

	VkBufferImageCopy imgCopy{};
	imgCopy.bufferImageHeight = 0;
	imgCopy.bufferRowLength = 0;
	imgCopy.bufferOffset = 0;

	imgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imgCopy.imageSubresource.mipLevel = 0;
	imgCopy.imageSubresource.baseArrayLayer = 0;
	imgCopy.imageSubresource.layerCount = 1;

	imgCopy.imageOffset = { 0, 0, 0 };
	imgCopy.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imgCopy
	);

	device.endSingleTimeCommands(commandBuffer);
}

void FLTexture::createImageView(){
	VkImageViewCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.image = textureImage;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	VK_CHECK_RESULT(vkCreateImageView(device.getDevice(), &createInfo, nullptr, &imageView),
		"Failed to create image view for texture image");
}

void FLTexture::createSampler(){
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties props{};
	vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &props);
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
	FL_DEBUG("Max sampler anisotropy %f", samplerInfo.maxAnisotropy);

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	VK_CHECK_RESULT(vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &imageSampler),
		"Failed to create image sampler");
}
