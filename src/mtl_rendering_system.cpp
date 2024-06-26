//
//  mtl_engine.cpp
//  MetalTest
//
//  Created by Mac on 4/8/24.
//

#include "mtl_rendering_system.hpp"
#include "camera.hpp"
#include <simd/simd.h>
#include <iostream>
#include <string>

MTLRenderingSystem::MTLRenderingSystem(ComponentManager* manager, MTL::Device* metalDevice, GLFWwindow* glfwWindow, int width, int height) {
    this->componentManager = manager;
    this->metalDevice = metalDevice;
    this->glfwWindow = glfwWindow;
    this->displayWidth = width;
    this->displayHeight = height;
}

void MTLRenderingSystem::init() {
    initWindow();
    
    createBuffers();
    createDefaultLibrary();
    createCommandQueue();
    createRenderPipeline();
    createDepthAndMSAATextures();
    createRenderPassDescriptor();
}

void MTLRenderingSystem::run(Camera* camera, RenderMode renderMode) {
    pPool = NS::AutoreleasePool::alloc()->init();
    
    std::optional<MeshInfo> meshInfo = componentManager->get_geometry(0);
    std::optional<Texture> textureData = componentManager->get_texture(0);
    
    
    metalDrawable = layer->nextDrawable();
    metalCommandBuffer = metalCommandQueue->commandBuffer();
    MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
    updateRenderPassDescriptor();
    for (int i = 0; i < componentManager->getNumEntities(); i++) {
        encodeRenderCommand(renderCommandEncoder, i, camera);
    }

    memcpy(transformationBuffer->contents(), &transformationData, sizeof(transformationData));
    memcpy(cameraTransformBuffer->contents(), &camera->position, sizeof(simd::float3));
    renderCommandEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    renderCommandEncoder->setCullMode(MTL::CullModeBack);
    renderCommandEncoder->setRenderPipelineState(metalRenderPSO);
    renderCommandEncoder->setDepthStencilState(depthStencilState);
    renderCommandEncoder->setVertexBuffer(meshInfo->vertexBuffer, 0, 0);
    renderCommandEncoder->setVertexBuffer(transformationBuffer, 0, 1);
    renderCommandEncoder->setVertexBuffer(cameraTransformBuffer, 0, 2);
    // Translate from engine specific to API specific modes
    MTL::PrimitiveType typeTriangle;
    switch (renderMode)
    {
        case full:      
            typeTriangle = MTL::PrimitiveTypeTriangle;
            break;
        case wireframe:
            typeTriangle = MTL::PrimitiveTypeLine;
            break;
        default:
            typeTriangle = MTL::PrimitiveTypeTriangle;
    }
    NS::UInteger vertexStart = 0;
    
    if (textureData) renderCommandEncoder->setFragmentTexture(textureData->texture, 0);
    renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, meshInfo->numVerteces, componentManager->getNumEntities());
    
    renderCommandEncoder->pushDebugGroup(NS::String::string("ImGui", NS::ASCIIStringEncoding));
    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
    ImGui_ImplGlfw_NewFrame();
    drawMeshInfoWidget(meshInfo, renderCommandEncoder);
    drawCameraInfoWidget(camera, renderCommandEncoder);
    renderCommandEncoder->popDebugGroup();
    
    renderCommandEncoder->endEncoding();
    swapBuffers();
    pPool->release();
}

void MTLRenderingSystem::cleanup() {
    // componentManager will be cleaned up by SlothEngine
    glfwTerminate();
    // TODO: Should the MTL::Buffers* even be managed by the rendering system?
    transformationBuffer->release();
    cameraTransformBuffer->release();
    msaaRenderTargetTexture->release();
    depthTexture->release();
    renderPassDescriptor->release();
}


void MTLRenderingSystem::initWindow() {
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, frameBufferSizeCallback);
    
    layer = CA::MetalLayer::layer();
    layer->setDevice(metalDevice);
    layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    layer->setDrawableSize(CGSizeMake(displayWidth*2, displayHeight*2));
    GLFWBridge::AddLayerToWindow(glfwWindow, layer);
    metalDrawable = layer->nextDrawable();
}

// TODO: This should be refactored by registering a MeshInfo in componentManager
void MTLRenderingSystem::createTriangle() {
//    simd::float3 triangleVertices[] = {
//        {-0.5f, -0.5f, 0.0f},
//        { 0.5f, -0.5f, 0.0f},
//        { 0.0f,  0.5f, 0.0f}
//    };
//    
//    vertexBuffer = metalDevice->newBuffer(&triangleVertices,
//                                          sizeof(triangleVertices),
//                                          MTL::ResourceStorageModeShared);
//    
}

// TODO: This should be refactored by registering a MeshInfo in componentManager
void MTLRenderingSystem::createSquare() {
//    VertexData squareVertices[] {
//        {{-0.5, -0.5,  0.5, 1.0f}, {0.0f, 0.0f}},
//        {{-0.5,  0.5,  0.5, 1.0f}, {0.0f, 1.0f}},
//        {{ 0.5,  0.5,  0.5, 1.0f}, {1.0f, 1.0f}},
//        {{-0.5, -0.5,  0.5, 1.0f}, {0.0f, 0.0f}},
//        {{ 0.5,  0.5,  0.5, 1.0f}, {1.0f, 1.0f}},
//        {{ 0.5, -0.5,  0.5, 1.0f}, {1.0f, 0.0f}}
//    };

//    vertexBuffer = metalDevice->newBuffer(&squareVertices, sizeof(squareVertices), MTL::ResourceStorageModeShared);

//    anyaTexture = new Texture("assets/anya.jpg", metalDevice);
}

void MTLRenderingSystem::createDefaultLibrary() {
    metalDefaultLibrary = metalDevice->newDefaultLibrary();
    if (!metalDefaultLibrary) {
        std::cerr << "Failed to load default library.";
        std::exit(-1);
    }
}

void MTLRenderingSystem::createCommandQueue() {
    metalCommandQueue = metalDevice->newCommandQueue();
}

void MTLRenderingSystem::createRenderPipeline() {
    MTL::Function* vertexShader = metalDefaultLibrary->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
    assert(vertexShader);
    MTL::Function* fragmentShader = metalDefaultLibrary->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));
    assert(fragmentShader);
    
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setLabel(NS::String::string("Cube Rendering Pipeline", NS::ASCIIStringEncoding));
    renderPipelineDescriptor->setVertexFunction(vertexShader);
    renderPipelineDescriptor->setFragmentFunction(fragmentShader);
    assert(renderPipelineDescriptor);
    MTL::PixelFormat pixelFormat = layer->pixelFormat();
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);
    renderPipelineDescriptor->setSampleCount(sampleCount);
    renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
    
    NS::Error* error;
    metalRenderPSO = metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);
    if (metalRenderPSO == nullptr) {
        std::cerr << "Error creating render pipeline state: " << error << std::endl;
        std::exit(-1);
    }
    
    MTL::DepthStencilDescriptor* depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
    depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
    depthStencilDescriptor->setDepthWriteEnabled(true);
    depthStencilState = metalDevice->newDepthStencilState(depthStencilDescriptor);
    
    renderPipelineDescriptor->release();
    vertexShader->release();
    fragmentShader->release();
    depthStencilDescriptor->release();
}

void MTLRenderingSystem::draw(int entityId, Camera* camera, MTL::RenderCommandEncoder* encoder) {
    sendRenderCommand(entityId, camera, encoder);
}

void MTLRenderingSystem::sendRenderCommand(int entityId, Camera* camera, MTL::RenderCommandEncoder* renderCommandEncoder) {
    encodeRenderCommand(renderCommandEncoder, entityId, camera);
}

void MTLRenderingSystem::swapBuffers() {
    metalCommandBuffer->presentDrawable(metalDrawable);
    metalCommandBuffer->commit();
    metalCommandBuffer->waitUntilCompleted();
}

void MTLRenderingSystem::encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder, int entityId, Camera* camera) {
    std::optional<Transform> transform = componentManager->get_transform(entityId);
    matrix_float4x4 translationMatrix = matrix4x4_translation(transform->position);

    // TODO: Introduce physics system
//    float angleInDegrees = glfwGetTime()/2.0 * 90;
//    float angleInRadians = angleInDegrees * M_PI / 180.0f;
//    matrix_float4x4 rotationMatrix = matrix4x4_rotation(angleInRadians, 1.0, 1.0, 0.0);
//    matrix_float4x4 scaleMatrix = matrix4x4_scale(transform->scale);
//
//    matrix_float4x4 modelMatrix = simd_mul(translationMatrix, scaleMatrix);
//    modelMatrix = simd_mul(modelMatrix, rotationMatrix);
    matrix_float4x4 modelMatrix = translationMatrix;

    matrix_float4x4 viewMatrix = camera->generate_view_matrix();
    
    float aspectRatio = (layer->drawableSize().width / layer->drawableSize().height);

    matrix_float4x4 perspectiveMatrix = matrix_perspective_right_hand(camera->fov, aspectRatio, camera->nearZ, camera->farZ);

    transformationData[entityId] = { modelMatrix, viewMatrix, perspectiveMatrix };
}

void MTLRenderingSystem::frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    MTLRenderingSystem* engine = (MTLRenderingSystem*)glfwGetWindowUserPointer(window);
    engine->resizeFrameBuffer(width, height);
}

void MTLRenderingSystem::resizeFrameBuffer(int width, int height) {
    layer->drawableSize() = CGSizeMake(width, height);
    // Deallocate the textures if they have been created
    if (msaaRenderTargetTexture) {
        msaaRenderTargetTexture->release();
        msaaRenderTargetTexture = nullptr;
    }
    if (depthTexture) {
        depthTexture->release();
        depthTexture = nullptr;
    }
    createDepthAndMSAATextures();
    metalDrawable = layer->nextDrawable();
    updateRenderPassDescriptor();
}

void MTLRenderingSystem::createBuffers() {
    transformationBuffer = metalDevice->newBuffer(sizeof(transformationData), MTL::ResourceStorageModeShared);
    cameraTransformBuffer = metalDevice->newBuffer(sizeof(simd::float3), MTL::ResourceStorageModeShared);
}

void MTLRenderingSystem::createDepthAndMSAATextures() {
    MTL::TextureDescriptor* msaaTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    msaaTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
    msaaTextureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    msaaTextureDescriptor->setWidth(layer->drawableSize().width);
    msaaTextureDescriptor->setHeight(layer->drawableSize().height);
    msaaTextureDescriptor->setSampleCount(sampleCount);
    msaaTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);

    msaaRenderTargetTexture = metalDevice->newTexture(msaaTextureDescriptor);

    MTL::TextureDescriptor* depthTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
    depthTextureDescriptor->setTextureType(MTL::TextureType2DMultisample);
    depthTextureDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
    depthTextureDescriptor->setWidth(layer->drawableSize().width);
    depthTextureDescriptor->setHeight(layer->drawableSize().height);
    depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
    depthTextureDescriptor->setSampleCount(sampleCount);

    depthTexture = metalDevice->newTexture(depthTextureDescriptor);

    msaaTextureDescriptor->release();
    depthTextureDescriptor->release();
}

void MTLRenderingSystem::createRenderPassDescriptor() {
    renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPassDescriptor->depthAttachment();

    colorAttachment->setTexture(msaaRenderTargetTexture);
    colorAttachment->setResolveTexture(metalDrawable->texture());
    colorAttachment->setLoadAction(MTL::LoadActionClear);
    colorAttachment->setClearColor(MTL::ClearColor(41.0f/255.0f, 42.0f/255.0f, 48.0f/255.0f, 1.0));
    colorAttachment->setStoreAction(MTL::StoreActionMultisampleResolve);

    depthAttachment->setTexture(depthTexture);
    depthAttachment->setLoadAction(MTL::LoadActionClear);
    depthAttachment->setStoreAction(MTL::StoreActionDontCare);
    depthAttachment->setClearDepth(1.0);
}

void MTLRenderingSystem::updateRenderPassDescriptor() {
    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPassDescriptor->depthAttachment();
    
    colorAttachment->setTexture(msaaRenderTargetTexture);
    colorAttachment->setResolveTexture(metalDrawable->texture());
    
    depthAttachment->setTexture(depthTexture);
}

void MTLRenderingSystem::drawMeshInfoWidget(std::optional<MeshInfo> meshInfo, MTL::RenderCommandEncoder* renderCommandEncoder) {
    ImGui::NewFrame();
    ImGui::Begin("MeshInfo Widget");                          // Create a window called "Hello, world!" and append into it.
    ImGui::SetWindowSize({400, 300});
    ImGui::SetWindowFontScale(2);
    ImGui::Text("Num Triangles: %d", meshInfo->numTriangles);               // Display some text (you can use a format strings too)
    ImGui::Text("Num Verteces: %d", meshInfo->numVerteces);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), metalCommandBuffer, renderCommandEncoder);
}

void MTLRenderingSystem::drawCameraInfoWidget(Camera *camera, MTL::RenderCommandEncoder *renderCommandEncoder) {
    ImGui::NewFrame();
    ImGui::Begin("Camera Widget");
    ImGui::SetWindowSize({600, 300});
    ImGui::SetWindowPos({100, 200});
    ImGui::SetWindowFontScale(2);
    ImGui::Text("Camera Position: \n\t%.2f, %.2f, %.2f", camera->position.x, camera->position.y, camera->position.z);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), metalCommandBuffer, renderCommandEncoder);
}
