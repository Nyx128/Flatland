#pragma once
#include "FLWindow.hpp"
#include <memory>

#include "../vulkan/FLDevice.hpp"
#include "../vulkan/FLSwapchain.hpp"
#include "../vulkan/FLPipeline.hpp"
#include "../vulkan/FLRenderer.hpp"
#include "FLGameObject.hpp"
#include "FLModel2D.hpp"

class Application{
public:
	Application();
	~Application();

	void createMemoryAllocator();
	void run();

private:
	std::shared_ptr<FLWindow> window;
	std::unique_ptr<FLDevice> device;
	std::unique_ptr<FLSwapchain> swapchain;
	std::unique_ptr<FLPipeline> graphicsPipeline;
	std::unique_ptr<FLRenderer> renderer;

	std::vector<FLGameObject> gameObjects;

	void initVulkan();
};
