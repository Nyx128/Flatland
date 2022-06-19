#include "application.hpp"
#include "logger.hpp"
#include "../vulkan/FLDevice.hpp"

Application::Application(gameInstance* _gameInst):gameInst(_gameInst) {
    window = std::make_unique<FLWindow>(gameInst->app_config.name, gameInst->app_config.width, gameInst->app_config.height);

    if (!gameInst->initialize(gameInst)) {
        FL_FATAL("game failed to initialize");
    }

    gameInst->resize(gameInst, window->getWidth(), window->getHeight());

}

Application::~Application(){

}

void Application::run(){

    FLDevice flDevice;

    if (!gameInst->update(gameInst, 0)) {
        FL_FATAL("failed to call the game's update function");
    }

    if (!gameInst->render(gameInst, 0)) {
        FL_FATAL("failed to call the game's render function");
    }

    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();
    }
}
