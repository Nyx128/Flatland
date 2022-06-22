#include "FLDevice.hpp"
#include "vulkan_asserts.hpp"

FLDevice::FLDevice() {
	FL_TRACE("FLDevice constructor called");
	pickPhysicalDevice();
}

FLDevice::~FLDevice(){
	FL_TRACE("FLDevice destructor called");
}

void FLDevice::pickPhysicalDevice(){
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, nullptr);
	std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
	vkEnumeratePhysicalDevices(instance.getInstance(), &deviceCount, physicalDevices.data());

	for (const auto& device : physicalDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}


	FL_ASSERT_MSG(!(physicalDevice == VK_NULL_HANDLE), "Failed to find suitable GPU");

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
	FL_INFO("Physical Device Selected: %s", deviceProps.deviceName);

}

bool FLDevice::isDeviceSuitable(VkPhysicalDevice device){
	auto indices = findQueueFamilies(device);

	return indices.isComplete();
}

FLDevice::QueueFamilyIndices FLDevice::findQueueFamilies(VkPhysicalDevice device) {

	QueueFamilyIndices indices;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> familyProps(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, familyProps.data());

	uint32_t i = 0;
	for (const auto& family : familyProps) {
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if (indices.isComplete()) { break; }
		i++;
	}
	
	return indices;
}
