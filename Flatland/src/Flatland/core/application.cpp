#include "application.hpp"
#include "logger.hpp"
#include "vulkan_asserts.hpp"
#include "FLModel2D.hpp"
#include "FLVertexBuffer.hpp"
#include "FLInputManager.hpp"


Application::Application() {
    

}

Application::~Application(){

}

void Application::initVulkan(){
    window = std::make_shared<FLWindow>("Sandbox", 1280, 720);
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
    if (FLInputManager::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { printf("left mb pressed\n"); }
}

void Application::run(){

    initVulkan();
    const std::vector<FLModel2D::Vertex> vertices = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    FLModel2D triangle{ vertices };
    FLVertexBuffer buffer{*device, (void*)triangle.getVertices().data(), sizeof(FLModel2D::Vertex) * vertices.size() };
    
    renderer = std::make_unique<FLRenderer>(*device, *swapchain, *graphicsPipeline, buffer);

    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        glfwPollEvents();
        processInput();
        renderer->draw();
    }

}

