#pragma once
#include "FLWindow.hpp"
#include <memory>

#include "../vulkan/FLDevice.hpp"
#include "../vulkan/FLSwapchain.hpp"
#include "../vulkan/FLPipeline.hpp"
#include "../vulkan/FLRenderer.hpp"
#include "FLModel2D.hpp"

#include "ECS/FLEntityManager.hpp"
#include "ECS/FLComponentManager.hpp"
#include "ECS/FLSystemManager.hpp"


class Application{
public:
	Application();
	~Application();
	void run();

private:
	std::shared_ptr<FLWindow> window;
	std::unique_ptr<FLDevice> device;
	std::unique_ptr<FLSwapchain> swapchain;
	std::unique_ptr<FLPipeline> graphicsPipeline;
	std::unique_ptr<FLRenderer> renderer;

	void initVulkan();
};
