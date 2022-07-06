#pragma once
#include "../core.hpp"
#include "FLWindow.hpp"
#include "gameInstance.hpp"
#include <memory>

#include "../vulkan/FLDevice.hpp"
#include "../vulkan/FLSwapchain.hpp"
#include "../vulkan/FLPipeline.hpp"

class FL_API Application{
public:
	Application(gameInstance* _gameInst);
	~Application();

	void run();

private:
	std::shared_ptr<FLWindow> window;
	std::unique_ptr<FLDevice> device;
	std::unique_ptr<FLSwapchain> swapchain;
	std::unique_ptr<FLPipeline> graphicsPipeline;
	gameInstance* gameInst;

	void initVulkan();
};
