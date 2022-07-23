#include "application.hpp"
#include "logger.hpp"

Application::Application(gameInstance* _gameInst):gameInst(_gameInst) {
    window = std::make_shared<FLWindow>(gameInst->app_config.name, gameInst->app_config.width, gameInst->app_config.height);
    device = std::make_unique<FLDevice>(window);
    swapchain = std::make_unique<FLSwapchain>(*device);

    FLPipeline::FLPipelineBuilder pipelineBuilder;
    FLPipeline::FLPipelineConfig pipelineConfig;
    FLPipeline::createDefaultPipelineConfig(pipelineConfig);
    pipelineBuilder.pipelineConfig = pipelineConfig;
    pipelineBuilder.vertPath = "src/shaders/unlit.vert.spv";
    pipelineBuilder.fragPath = "src/shaders/unlit.frag.spv";

    graphicsPipeline = std::make_unique<FLPipeline>(*device, *swapchain, pipelineBuilder);

    FLRenderer::FLRendererConfig rendererConfig{};
    renderer = std::make_unique<FLRenderer>(*device, *swapchain, *graphicsPipeline, rendererConfig);

    if (!gameInst->initialize(gameInst)) {
        FL_FATAL("game failed to initialize");
    }

    gameInst->resize(gameInst, window->getWidth(), window->getHeight());

}

Application::~Application(){

}

void Application::run(){

    initVulkan();


    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();

        if (!gameInst->update(gameInst, 0)) {
            FL_FATAL("failed to call the game's update function");
        }

        if (!gameInst->render(gameInst, 0)) {
            FL_FATAL("failed to call the game's render function");
        }

        renderer->draw();
    }
}

void Application::initVulkan(){
    
}
