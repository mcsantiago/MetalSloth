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
    texture = new Texture("assets/anya.jpg", metalDevice);
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

void SlothEngine::loadScene() {
    // TODO: Load this from a file
    lastFrameTime = std::chrono::high_resolution_clock::now();
    
    simd::float3 startingPosition = simd::float3 {1, 1, -3};
    simd::float3 startingRotation = simd::float3 {0, 0, 0};
    simd::float3 startingScale    = simd::float3 {1, 1, 1};
    Transform transform = Transform{startingPosition, startingRotation, startingScale};
    
    simd::float3 acceleration = simd::float3 {0, 0, 0};
    simd::float3 velocity = simd::float3 {0, 0, 0};
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
    
    Texture* texture = new Texture("assets/anya.jpg", metalDevice);

    componentManager->register_transform(1, transform);
    componentManager->register_kinetic_physical_properties(1, kinetics);
    componentManager->register_texture(1, texture);

    cubeVertexBuffer = metalDevice->newBuffer(&data, sizeof(data), MTL::ResourceStorageModeShared);
    transformationBuffer = metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared);
    componentManager->register_geometry(1, cubeVertexBuffer);
}

void SlothEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        std::chrono::time_point now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
        float deltaTime = duration.count() / 1000.0;
        // TODO: Generally we want to iterate through all of the entities and update if needed for all systems
        physicsSystem->update_entity(1, deltaTime);
        renderingSystem->run();
        glfwPollEvents();
        lastFrameTime = now;
    }
}

void SlothEngine::cleanup() {
    renderingSystem->cleanup();
    physicsSystem->cleanup();
    metalDevice->release();
    delete componentManager;
    delete renderingSystem;
    delete physicsSystem;
    delete texture;
}
