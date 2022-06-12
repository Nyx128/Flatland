#include "Flatland.hpp"
#include <stdlib.h>

int main() {
	Application::ApplicationConfig app_config;
	app_config.name = "Flatland";
	app_config.width = 1280;
	app_config.height = 720;

	Application application(app_config);
	application.run();
}