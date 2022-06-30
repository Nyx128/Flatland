#pragma once
#include "../core.hpp"
#include "FLWindow.hpp"
#include "gameInstance.hpp"
#include <memory>

#include "../vulkan/FLDevice.hpp"
#include "../vulkan/FLSwapchain.hpp"

class FL_API Application{
public:
	Application(gameInstance* _gameInst);
	~Application();

	void run();

private:
	std::shared_ptr<FLWindow> window;
	std::unique_ptr<FLDevice> device;
	std::unique_ptr<FLSwapchain> swapchain;
	gameInstance* gameInst;

	void initVulkan();
};
