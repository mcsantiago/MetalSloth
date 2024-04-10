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

#include "vertex_data.h"
#include "texture.hpp"
#include "glfw_bridge.hpp"
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
    
    void createSquare();
    void createTriangle();
    void createDefaultLibrary();
    void createCommandQueue();
    void createRenderPipeline();
    
    void encodeRenderCommand(MTL::RenderCommandEncoder* renderEncoder);
    void sendRenderCommand();
    void draw();
    
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
    MTL::Buffer* triangleVertexBuffer;
    MTL::Buffer* squareVertexBuffer;
    
    Texture* anyaTexture;
    NS::AutoreleasePool* pPool;
};


#endif /* mtl_engine_h */
