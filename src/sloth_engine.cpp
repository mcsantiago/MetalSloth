//
//  sloth_engine.cpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#include "sloth_engine.hpp"


void SlothEngine::init() {
    initDevice();
    initWindow();
    initSystems();
    loadScene();
}

void SlothEngine::initDevice() {
    metalDevice = MTL::CreateSystemDefaultDevice();
}

void SlothEngine::initSystems() {
    componentManager = new ComponentManager();
    renderingSystem = new MTLEngine(componentManager, metalDevice, glfwWindow);
    physicsSystem = new PhysicsSystem(componentManager, metalDevice);
    renderingSystem->init();
}

void SlothEngine::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Metal Engine", nullptr, nullptr);
    if (glfwWindow == nullptr) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}

void SlothEngine::createCamera() {
    //    Camera(simd::float3 position = simd::float3{0.0f, 0.0f,  0.0f},
    //           simd::float3 up       = simd::float3{0.0f, 1.0f,  0.0f},
    //           simd::float3 right    = simd::float3{1.0f, 0.0f,  0.0f},
    //           simd::float3 front    = simd::float3{0.0f, 0.0f, -1.0f});
    int numEntities = componentManager->getNumEntities();
    simd::float3 position   = simd::float3{(numEntities/2.0f) + numEntities, 0.0f,  10.0f};
    simd::float3 up         = simd::float3{0.0f, 1.0f,  0.0f};
    simd::float3 right      = simd::float3{1.0f, 0.0f,  0.0f};
    simd::float3 front      = simd::float3{0.0f, 0.0f,  -1.0f};
    camera = new Camera(position, up, right, front);
}

void SlothEngine::loadScene() {
    createCamera();
    for (int i = 0; i < componentManager->getNumEntities(); i++) {
        loadObject(i,
                  {i * 4.0f, 1, -3},
                  {0, 0, 0},
                  {1, 1, 1},
                  {0, 0, 0},
                  {0, 0, 0});
    }
}

void SlothEngine::loadObject(int entityId, 
                             simd::float3 position,
                             simd::float3 rotation,
                             simd::float3 scale,
                             simd::float3 velocity,
                             simd::float3 acceleration) {
    
    // TODO: Load this from a file
    lastFrameTime = std::chrono::high_resolution_clock::now();
    
    Transform transform = Transform{position, rotation, scale};
    
    float mass = 1;
    KineticPhysicalProperties kinetics = KineticPhysicalProperties{mass, velocity, acceleration};
    
    VertexData data[] = {
            // Front face
            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
            {{0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
            {{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},

            // Back face
            {{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
            {{-0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
            {{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
            {{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

            // Top face
            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},
            {{0.5, 0.5, 0.5, 1.0}, {1.0, 0.0}},
            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},

            // Bottom face
            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
            {{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
            {{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 1.0}},
            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

            // Left face
            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
            {{-0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
            {{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
            {{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},

            // Right face
            {{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
            {{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
            {{0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
            {{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
        };
    
    componentManager->register_transform(entityId, transform);
    componentManager->register_kinetic_physical_properties(entityId, kinetics);
    
    Texture* texture = new Texture("assets/anya.jpg", metalDevice);
    componentManager->register_texture(entityId, texture);

    
    MTL::Buffer* cubeVerteces = metalDevice->newBuffer(&data, sizeof(data), MTL::ResourceStorageModeShared);
    cubeVertexBuffers.insert({entityId, cubeVerteces});
    transformationBuffers.insert({entityId, metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared)});
    componentManager->register_geometry(entityId, cubeVerteces);

}

void SlothEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        std::chrono::time_point now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
        float deltaTime = duration.count() / 1000.0;
        physicsSystem->update_loaded_entities(deltaTime);
        renderingSystem->run(camera);
        glfwPollEvents();
        lastFrameTime = now;
    }
}

void SlothEngine::cleanup() {
    renderingSystem->cleanup();
    physicsSystem->cleanup();
    metalDevice->release();
    
//    for (auto it = cubeVertexBuffers.begin(); it != cubeVertexBuffers.end(); ++it) {
//        if (it->second != nullptr)
//            delete it->second;
//    }
    
    delete camera;
    delete componentManager;
    delete renderingSystem;
    delete physicsSystem;
}
