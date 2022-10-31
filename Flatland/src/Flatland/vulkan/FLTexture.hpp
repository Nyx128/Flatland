#pragma once
#include "FLDevice.hpp"
#include "stb_image/stb_image.hpp"

class FLTexture {
public:
	FLTexture(FLDevice& _device, const char* filepath);
	~FLTexture();

	FLTexture(const FLTexture&) = delete;
	FLTexture& operator=(const FLTexture&) = delete;

	VkSampler& getSampler() { return imageSampler; }
	VkImageView& getImageView() { return imageView; }
private:
	FLDevice& device;
	const char* filepath;
	VkDeviceSize imageSize;
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VmaAllocation textureImageAlloc;
	VkImageView imageView;
	VkSampler imageSampler;

	void getImage(const char* filepath);
	void createImage();
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImg(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createImageView();
	void createSampler();
};
