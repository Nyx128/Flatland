#include "application.hpp"

Application::Application(ApplicationConfig app_config){
    window = std::make_unique<FLWindow>(app_config.name, app_config.width, app_config.height);
}

Application::~Application(){

}

void Application::run(){
    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();
    }
}
