#pragma once
#include "../core.hpp"
#include "FLWindow.hpp"
#include "gameInstance.hpp"
#include <memory>

#include "../vulkan/FLDevice.hpp"
#include "../vulkan/FLSwapchain.hpp"
#include "../vulkan/FLPipeline.hpp"
#include "../vulkan/FLRenderer.hpp"

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
	std::unique_ptr<FLRenderer> renderer;
	gameInstance* gameInst;

	void initVulkan();
};
