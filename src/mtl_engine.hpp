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
#include "camera.hpp"
#include "component_manager.hpp"
#include <stb/stb_image.h>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <filesystem>

enum RenderMode { full, wireframe };

class MTLEngine {
public:
    MTLEngine(ComponentManager* manager, MTL::Device* metalDevice, GLFWwindow* glfwWindow);
    void init();
    void run(Camera* camera, RenderMode renderMode);
    void swapBuffers();
    void cleanup();
    
private:
    void initWindow();
    
    void createCube();
    void createSquare();
    void createTriangle();
    void createBuffers();
    void createDepthAndMSAATextures();
    void createRenderPassDescriptor();
    void createDefaultLibrary();
    void createCommandQueue();
    void createRenderPipeline();
    
    void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder, int entityId, Camera* camera);
    void sendRenderCommand(int entityId, Camera* camera, MTL::RenderCommandEncoder* renderEncoder);
    void draw(int entityId, Camera* camera, MTL::RenderCommandEncoder* renderEncoder);
    void updateRenderPassDescriptor();
    
    static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
    void resizeFrameBuffer(int width, int height);
    
    TransformationData transformationData[1];
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
    
    MTL::Buffer* vertexBuffer;
    MTL::Buffer* transformationBuffer;
    int sampleCount = 4; // MSAA Sample count
    
    NS::AutoreleasePool* pPool;
    
    Camera* camera;
    ComponentManager* componentManager;
};


#endif /* mtl_engine_h */
