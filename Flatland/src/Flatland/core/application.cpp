#include "application.hpp"
#include "logger.hpp"
#include "vulkan_asserts.hpp"
#include "FLModel2D.hpp"
#include "FLVertexBuffer.hpp"
#include "FLInputManager.hpp"
#include "ECS/components/Renderable.hpp"

#include <chrono>

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1280
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 720
#endif

 FLEntityManager flEntityManager;
 FLComponentManager flComponentManager;
 FLSystemManager flSystemManager;

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

class TestSystem : public FLSystem {
public:
    TestSystem() {
    
    };
    ~TestSystem() {}
    void Update(float dt) {
        for (auto& entity : mEntities) {
            auto& transform = flComponentManager.GetComponent<Transform2D>(entity);

            if (FLInputManager::isKeyPressed(FL_KEY_UP)) {
                transform.position.y += -2.0f * dt;
            }
            if (FLInputManager::isKeyPressed(FL_KEY_DOWN)) {
                transform.position.y += 2.0f * dt;
            }
            if (FLInputManager::isKeyPressed(FL_KEY_RIGHT)) {
                transform.position.x += 2.0f * dt;
            }
            if (FLInputManager::isKeyPressed(FL_KEY_LEFT)) {
                transform.position.x += -2.0f * dt;
            }
            if (FLInputManager::isKeyPressed(FL_KEY_SPACE)) {
                transform.rotation += 1.0f * dt;
            }
        }
    }
};

void Application::run(){

    initVulkan();

    renderer = std::make_unique<FLRenderer>(*device, *swapchain, *graphicsPipeline);

    float dt = 0.0f;

    flComponentManager.RegisterComponent<Transform2D>();
    flComponentManager.RegisterComponent<Renderable>();
    auto testSys = flSystemManager.RegisterSystem<TestSystem>();

    Signature signature;
    signature.set(flComponentManager.GetComponentType<Transform2D>());
    signature.set(flComponentManager.GetComponentType<Renderable>());
    flSystemManager.SetSignature<TestSystem>(signature);

    FLEntity box = flEntityManager.CreateEntity();
    flComponentManager.AddComponent(box, Transform2D{});
    std::shared_ptr<FLModel2D> square = FLModel2D::createSquare(*device, glm::vec2(0.0f), glm::vec3(0.05f, 0.1f, 0.9f));
    flComponentManager.AddComponent(box, Renderable{square});

    testSys->mEntities.emplace(box);

    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        auto startTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        processInput();

        renderer->draw(testSys->mEntities);
        testSys->Update(dt);

        auto stopTime = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();

    }
    flComponentManager.cleanup();
}

