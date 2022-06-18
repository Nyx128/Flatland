#pragma once
#include "vulkan/vulkan.h"

class FLInstance {
public:
	FLInstance(const char* appName);
	~FLInstance();

	FLInstance(const FLInstance&) = delete;
	FLInstance& operator=(const FLInstance&) = delete;

private:
	VkInstance instance;
};
