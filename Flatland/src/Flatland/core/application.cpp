#include "application.hpp"
#include "logger.hpp"
#include "vulkan_asserts.hpp"
#include "FLModel2D.hpp"
#include "FLVertexBuffer.hpp"
#include "FLInputManager.hpp"
#include "ECS/components/Renderable.hpp"
#include "ECS/components/Transform2D.hpp"
#include "ECS/components/ImageTexture.hpp"
#include "FLUniformBufferArray.hpp"

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


    float dt = 0.0f;

    flComponentManager.RegisterComponent<Transform2D>();
    flComponentManager.RegisterComponent<Renderable>();
    flComponentManager.RegisterComponent<ImageTexture>();
    auto testSys = flSystemManager.RegisterSystem<TestSystem>();

    Signature signature;
    signature.set(flComponentManager.GetComponentType<Transform2D>());
    signature.set(flComponentManager.GetComponentType<Renderable>());
    flSystemManager.SetSignature<TestSystem>(signature);

    FLEntity box = flEntityManager.CreateEntity();
    flComponentManager.AddComponent(box, Transform2D{});
    auto& transformBox = flComponentManager.GetComponent<Transform2D>(box);
    transformBox.scale = glm::vec2(1.6, 0.9);
    transformBox.position = glm::vec2(-0.5, 0.0);
    std::shared_ptr<FLModel2D> square = FLModel2D::createSquare(*device, glm::vec2(0.0f), glm::vec3(0.05f, 0.1f, 0.9f));
    flComponentManager.AddComponent(box, Renderable{square});
    std::shared_ptr<FLTexture> imgTexZoro = std::make_shared<FLTexture>(*device, "external/images/zoro.png");
    flComponentManager.AddComponent(box, ImageTexture{ imgTexZoro });

    FLEntity box_2 = flEntityManager.CreateEntity();
    flComponentManager.AddComponent(box_2, Transform2D{});
    flComponentManager.AddComponent(box_2, Renderable{ square });
    std::shared_ptr<FLTexture> imgTexLaw = std::make_shared<FLTexture>(*device, "external/images/law.png");
    flComponentManager.AddComponent(box_2, ImageTexture{ imgTexLaw });

    testSys->mEntities.emplace(box);
    testSys->mEntities.emplace(box_2);

    renderer = std::make_unique<FLRenderer>(*device, *swapchain, *graphicsPipeline, testSys->mEntities);
    while (!glfwWindowShouldClose(window->getWindowPointer())) {
        auto startTime = std::chrono::high_resolution_clock::now();
        glfwPollEvents();
        processInput();

        renderer->draw();
        testSys->Update(dt);

        auto stopTime = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();

    }
    flComponentManager.cleanup();
}

