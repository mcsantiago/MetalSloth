//
//  sloth_engine.hpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#ifndef sloth_engine_hpp
#define sloth_engine_hpp

#define GLFW_INCLUDE_NONE
#import <GLFW/glfw3.h>

#include <Metal/Metal.hpp>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "glfw_bridge.hpp"
#include "mtl_engine.hpp"
#include "physics_engine.hpp"
#include "vertex_data.h"
#include "texture.hpp"
#include "ufbx/ufbx.h"

class SlothEngine {
private:
    MTLEngine* renderingSystem;
    PhysicsSystem* physicsSystem;
    ComponentManager* componentManager;
    
    RenderMode renderMode = full;
    GLFWwindow* glfwWindow;
    MTL::Device* metalDevice;
    
    // TODO: Maybe a map of buffers?
    std::unordered_map<int, MTL::Buffer*> transformationBuffers;
    Texture* texture;
    MTL::Buffer* vertexBuffer;
    
    NS::AutoreleasePool* pPool;
    Camera* camera;
    float lastX, lastY;
    
//    VertexData vertexData[36] = {
//            // Front face
//            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
//            {{0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
//            {{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
//            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
//
//            // Back face
//            {{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//            {{-0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
//            {{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
//            {{0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//
//            // Top face
//            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},
//            {{0.5, 0.5, 0.5, 1.0}, {1.0, 0.0}},
//            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
//            {{-0.5, 0.5, 0.5, 1.0}, {0.0, 0.0}},
//
//            // Bottom face
//            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//            {{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
//            {{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, -0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, -0.5, 0.5, 1.0}, {0.0, 1.0}},
//            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//
//            // Left face
//            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//            {{-0.5, -0.5, 0.5, 1.0}, {1.0, 0.0}},
//            {{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, 0.5, 0.5, 1.0}, {1.0, 1.0}},
//            {{-0.5, 0.5, -0.5, 1.0}, {0.0, 1.0}},
//            {{-0.5, -0.5, -0.5, 1.0}, {0.0, 0.0}},
//
//            // Right face
//            {{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
//            {{0.5, -0.5, -0.5, 1.0}, {1.0, 0.0}},
//            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, 0.5, -0.5, 1.0}, {1.0, 1.0}},
//            {{0.5, 0.5, 0.5, 1.0}, {0.0, 1.0}},
//            {{0.5, -0.5, 0.5, 1.0}, {0.0, 0.0}},
//        };
    

    std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime;

    void initWindow();
    void initDevice();
    void initSystems();
    void initGlfwCallbacks();
    
    // Model loading
    void loadFbxFile(const char *filename);
    void loadPolygons(int entityId, ufbx_mesh *mesh);
    void loadObject(int entityId,
                    simd::float3 position,
                    simd::float3 rotation,
                    simd::float3 scale,
                    simd::float3 velocity,
                    simd::float3 acceleration);
    
    void createCamera();
    
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);


public:
    void init();
    void loadScene();
    void run();
    void cleanup();
};

#endif /* sloth_engine_hpp */
