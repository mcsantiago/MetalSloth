//
//  mtl_engine.hpp
//  MetalTest
//
//  Created by Mac on 4/8/24.
//

#ifndef mtl_engine_h
#define mtl_engine_h

#define GLFW_INCLUDE_NONE
#import <GLFW/glfw3.h>

#include "AAPLMathUtilities.h"
#include "vertex_data.h"
#include "texture.hpp"
#include "glfw_bridge.hpp"
#include "camera.hpp"
#include "component_manager.hpp"
#include <stb/stb_image.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <filesystem>

class MTLEngine {
public:
    void init();
    void run();
    void cleanup();
    
private:
    void initDevice();
    void initWindow();
    void initComponentManager();
    
    void createCube();
    void createSquare();
    void createTriangle();
    void createBuffers();
    void createDepthAndMSAATextures();
    void createRenderPassDescriptor();
    void createDefaultLibrary();
    void createCommandQueue();
    void createRenderPipeline();
    void createCamera();
    
    void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder);
    void sendRenderCommand();
    void draw();
    void updateRenderPassDescriptor();
    
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
    void resizeFrameBuffer(int width, int height);
    
    MTL::Device* metalDevice;
    GLFWwindow* glfwWindow;
    CA::MetalDrawable* metalDrawable;
    CA::MetalLayer* layer;
    
    MTL::Library* metalDefaultLibrary;
    MTL::CommandQueue* metalCommandQueue;
    MTL::CommandBuffer* metalCommandBuffer;
    MTL::RenderPipelineState* metalRenderPSO;
    MTL::DepthStencilState* depthStencilState;
    MTL::RenderPassDescriptor* renderPassDescriptor;
    MTL::Texture* msaaRenderTargetTexture = nullptr;
    MTL::Texture* depthTexture;
    
    MTL::Buffer* triangleVertexBuffer;
    MTL::Buffer* squareVertexBuffer;
    MTL::Buffer* cubeVertexBuffer;
    MTL::Buffer* transformationBuffer;
    int sampleCount = 4;
    
    Texture* anyaTexture;
    NS::AutoreleasePool* pPool;
    
    Camera* camera;
    ComponentManager* componentManager;
};


#endif /* mtl_engine_h */
