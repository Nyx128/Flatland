#include "application.hpp"
#include "logger.hpp"

Application::Application(gameInstance* _gameInst):gameInst(_gameInst) {
    window = std::make_unique<FLWindow>(gameInst->app_config.name, gameInst->app_config.width, gameInst->app_config.height);

    gameInst->initialize(gameInst);
    gameInst->resize(gameInst, window->getWidth(), window->getHeight());

}

Application::~Application(){

}

void Application::run(){

    gameInst->update(gameInst, 0);

    gameInst->render(gameInst, 0);

    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();
    }
}
