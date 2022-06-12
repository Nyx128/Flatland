#pragma once
#include "../core.hpp"
#include "FLWindow.hpp"
#include <memory>

class FL_API Application{
public:
	struct ApplicationConfig {
		uint32_t width;
		uint32_t height;
		const char* name;
	};

	Application(ApplicationConfig app_config);
	~Application();

	void run();

private:
	std::unique_ptr<FLWindow> window;
};
