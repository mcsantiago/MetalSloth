//
//  sloth_engine.cpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#include "sloth_engine.hpp"
#include <cmath>

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
    int c = cbrt(static_cast<double>(componentManager->getNumEntities()));
    simd::float3 position   = simd::float3{c * 1.0f, c * 1.0f,  c * 3.0f};
    simd::float3 target     = simd::float3{0.0, 0.0f, 0.0f};
    camera = new Camera(position, target);
}

void SlothEngine::loadScene() {
    createCamera();
    int c = cbrt(static_cast<double>(componentManager->getNumEntities()));
    texture = new Texture("assets/anya.jpg", metalDevice);
    vertexBuffer = metalDevice->newBuffer(&vertexData, sizeof(vertexData), MTL::ResourceStorageModeShared);
    int n = 0;
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < c; j++) {
            for (int k = 0; k < c; k++) {
                loadObject(n++,
                          {i * 2.0f, j * 2.0f, k * 2.0f},
                          {0, 0, 0},
                          {1, 1, 1},
                          {0, 0, 0},
                          {0, 0, 0});
            }
        }
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
    
    
    componentManager->register_transform(entityId, transform);
    componentManager->register_kinetic_physical_properties(entityId, kinetics);
    componentManager->register_texture(entityId, texture);
    componentManager->register_geometry(entityId, vertexBuffer);
    
//    transformationBuffers.insert({entityId, metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared)});
}

void SlothEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        std::chrono::time_point now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
        float deltaTime = duration.count() / 1000.0;
//        physicsSystem->update_loaded_entities(deltaTime);
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
    delete texture;
}
