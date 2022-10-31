#include "component.hpp"
#include "vulkan/FLTexture.hpp"
#include <vector>
#include <memory>

struct ImageTexture {
	std::shared_ptr<FLTexture> texture;
	std::vector<VkDescriptorSet> descriptorSets;
};