//
//  sloth_engine.hpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#ifndef sloth_engine_hpp
#define sloth_engine_hpp

#include <Metal/Metal.hpp>
#include <chrono>
#include <unordered_map>
#include "mtl_engine.hpp"
#include "physics_engine.hpp"
#include "vertex_data.h"
#include "texture.hpp"

class SlothEngine {
private:
    MTLEngine* renderingSystem;
    PhysicsSystem* physicsSystem;
    ComponentManager* componentManager;
    
    GLFWwindow* glfwWindow;
    MTL::Device* metalDevice;
    
    // TODO: Maybe a map of buffers?
    std::unordered_map<int, MTL::Buffer*> cubeVertexBuffers;
    std::unordered_map<int, MTL::Buffer*> transformationBuffers;
    
    NS::AutoreleasePool* pPool;
    Camera* camera;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

    void initWindow();
    void initDevice();
    void initSystems();
    void loadObject(int entityId, 
                    simd::float3 position,
                    simd::float3 rotation,
                    simd::float3 scale,
                    simd::float3 velocity,
                    simd::float3 acceleration);
    void createCamera();


public:
    void init();
    void loadScene();
    void run();
    void cleanup();
};

#endif /* sloth_engine_hpp */
