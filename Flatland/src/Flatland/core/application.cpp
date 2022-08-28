#include "application.hpp"
#include "logger.hpp"
#include "vulkan_asserts.hpp"
#include "FLModel2D.hpp"
#include "FLVertexBuffer.hpp"
#include "FLInputManager.hpp"

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1280
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 720
#endif

Application::Application() {
    

}

Application::~Application(){

}

void Application::initVulkan(){
    window = std::make_shared<FLWindow>("Sandbox", WINDOW_WIDTH, WINDOW_HEIGHT);
    device = std::make_unique<FLDevice>(window);
    
    swapchain = std::make_unique<FLSwapchain>(*device);

    FLPipeline::FLPipelineBuilder pipelineBuilder;
    FLPipeline::FLPipelineConfig pipelineConfig;
    FLPipeline::createDefaultPipelineConfig(pipelineConfig);
    pipelineBuilder.pipelineConfig = pipelineConfig;
    pipelineBuilder.vertPath = "src/Flatland/shaders/unlit.vert.spv";
    pipelineBuilder.fragPath = "src/Flatland/shaders/unlit.frag.spv";

    graphicsPipeline = std::make_unique<FLPipeline>(*device, *swapchain, pipelineBuilder);
}

void processInput() {
    
}

void Application::run(){

    initVulkan();
    const std::vector<FLModel2D::Vertex> vertices = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
    };
    std::shared_ptr<FLModel2D> triangle = std::make_shared<FLModel2D>(*device, vertices, indices);

    FLGameObject triObj = FLGameObject::createGameObject();
    triObj.model = triangle;
    triObj.tranform.position = { glm::vec2(0.0f, 0.0f) };
    triObj.tranform.scale = { glm::vec2(1.7f, 0.2f) };
    triObj.tranform.rotation = glm::radians(0.0f);

    gameObjects.push_back(std::move(triObj));

    renderer = std::make_unique<FLRenderer>(*device, *swapchain, *graphicsPipeline, gameObjects);

    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();
        processInput();
        renderer->draw();
    }

}

