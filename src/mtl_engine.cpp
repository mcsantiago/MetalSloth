//
//  mtl_engine.cpp
//  MetalTest
//
//  Created by Mac on 4/8/24.
//

#include "mtl_engine.hpp"
#include "camera.hpp"
#include <simd/simd.h>
#include <iostream>

void MTLEngine::init() {
    initDevice();
    initWindow();
    initComponentManager();
    
    createCube();
    createCamera();
    createBuffers();
    createDefaultLibrary();
    createCommandQueue();
    createRenderPipeline();
    createDepthAndMSAATextures();
    createRenderPassDescriptor();
}

void MTLEngine::initComponentManager() {
    componentManager = new ComponentManager();
}

void MTLEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        pPool = NS::AutoreleasePool::alloc()->init();
        metalDrawable = layer->nextDrawable();
        draw();
        pPool->release();
        glfwPollEvents();
    }
}

void MTLEngine::cleanup() {
    glfwTerminate();
    transformationBuffer->release();
    msaaRenderTargetTexture->release();
    depthTexture->release();
    renderPassDescriptor->release();
    metalDevice->release();
    delete componentManager;
    delete camera;
    delete anyaTexture;
}

void MTLEngine::initDevice() {
    metalDevice = MTL::CreateSystemDefaultDevice();
}

void MTLEngine::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindow = glfwCreateWindow(800, 600, "Metal Engine", nullptr, nullptr);
    if (glfwWindow == nullptr) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, frameBufferSizeCallback);
    
    layer = CA::MetalLayer::layer();
    layer->setDevice(metalDevice);
    layer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    layer->setDrawableSize(CGSizeMake(800, 600));
    GLFWBridge::AddLayerToWindow(glfwWindow, layer);
    metalDrawable = layer->nextDrawable();
}

void MTLEngine::createTriangle() {
    simd::float3 triangleVertices[] = {
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f},
        { 0.0f,  0.5f, 0.0f}
    };
    
    triangleVertexBuffer = metalDevice->newBuffer(&triangleVertices,
                                                  sizeof(triangleVertices),
                                                  MTL::ResourceStorageModeShared);
    
}

void MTLEngine::createSquare() {
    VertexData squareVertices[] {
        {{-0.5, -0.5,  0.5, 1.0f}, {0.0f, 0.0f}},
        {{-0.5,  0.5,  0.5, 1.0f}, {0.0f, 1.0f}},
        {{ 0.5,  0.5,  0.5, 1.0f}, {1.0f, 1.0f}},
        {{-0.5, -0.5,  0.5, 1.0f}, {0.0f, 0.0f}},
        {{ 0.5,  0.5,  0.5, 1.0f}, {1.0f, 1.0f}},
        {{ 0.5, -0.5,  0.5, 1.0f}, {1.0f, 0.0f}}
    };

    squareVertexBuffer = metalDevice->newBuffer(&squareVertices, sizeof(squareVertices), MTL::ResourceStorageModeShared);

    anyaTexture = new Texture("assets/anya.jpg", metalDevice);
}

void MTLEngine::createCube() {
    // Cube for use in a right-handed coordinate system with triangle faces
    // specified with a Counter-Clockwise winding order.
    VertexData cubeVertices[] = {
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
    
    
    simd::float3 startingPosition = simd::float3 {1, 1, -3};
    simd::float3 startingRotation = simd::float3 {0, 0, 0};
    simd::float3 startingScale    = simd::float3 {1, 1, 1};
    Transform transform = Transform{startingPosition, startingRotation, startingScale};
    
    simd::float3 acceleration = simd::float3 {0, 0, 0};
    simd::float3 velocity = simd::float3 {0, 0, 0};
    float mass = 1;
    KineticPhysicalProperties kinetics = KineticPhysicalProperties{mass, velocity, acceleration};
    
    componentManager->register_transform(1, transform);
    componentManager->register_kinetic_physical_properties(1, kinetics);
    

    cubeVertexBuffer = metalDevice->newBuffer(&cubeVertices, sizeof(cubeVertices), MTL::ResourceStorageModeShared);

    transformationBuffer = metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared);

    // Make sure to change working directory to Metal-Tutorial root
    // directory via Product -> Scheme -> Edit Scheme -> Run -> Options
    anyaTexture = new Texture("assets/anya.jpg", metalDevice);
}

void MTLEngine::createCamera() {
//    Camera(simd::float3 position = simd::float3{0.0f, 0.0f,  0.0f},
//           simd::float3 up       = simd::float3{0.0f, 1.0f,  0.0f},
//           simd::float3 right    = simd::float3{1.0f, 0.0f,  0.0f},
//           simd::float3 front    = simd::float3{0.0f, 0.0f, -1.0f});
    simd::float3 position   = simd::float3{0.0f, 0.0f,  1.0f};
    simd::float3 up         = simd::float3{0.0f, 1.0f,  0.0f};
    simd::float3 right      = simd::float3{1.0f, 0.0f,  0.0f};
    simd::float3 front      = simd::float3{0.0f, 0.0f,  -1.0f};
    camera = new Camera(position, up, right, front);
}

void MTLEngine::createDefaultLibrary() {
    metalDefaultLibrary = metalDevice->newDefaultLibrary();
    if (!metalDefaultLibrary) {
        std::cerr << "Failed to load default library.";
        std::exit(-1);
    }
}

void MTLEngine::createCommandQueue() {
    metalCommandQueue = metalDevice->newCommandQueue();
}

void MTLEngine::createRenderPipeline() {
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

void MTLEngine::draw() {
    sendRenderCommand();
}

void MTLEngine::sendRenderCommand() {
    metalCommandBuffer = metalCommandQueue->commandBuffer();
    
    updateRenderPassDescriptor();
    MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer->renderCommandEncoder(renderPassDescriptor);
    encodeRenderCommand(renderCommandEncoder);
    renderCommandEncoder->endEncoding();
    
    metalCommandBuffer->presentDrawable(metalDrawable);
    metalCommandBuffer->commit();
    metalCommandBuffer->waitUntilCompleted();
}

matrix_float4x4 get_model_matrix_next_timestep(int entityId, ComponentManager* manager, float deltaTime) {
    Transform* transform = manager->get_transform(entityId);
    KineticPhysicalProperties* kinetics = manager->get_kinetics(entityId);
    
    simd::float3& position = transform->position;
    simd::float3& velocity = kinetics->velocity;
    simd::float3& acceleration = kinetics->acceleration;
    
    float g_y = -9.81;
    acceleration = simd::float3{0, g_y, 0};
    velocity += acceleration;
    position += velocity;
    
    matrix_float4x4 translationMatrix = matrix4x4_translation(position);
    
    return translationMatrix;
}

void MTLEngine::encodeRenderCommand(MTL::RenderCommandEncoder* renderCommandEncoder) {
    // Moves the Cube 2 units down the negative Z-axis
    Transform* transform = componentManager->get_transform(1);
    KineticPhysicalProperties* kinetics = componentManager->get_kinetics(1);
    
    simd::float3& position = transform->position;
    simd::float3& velocity = kinetics->velocity;
    simd::float3& acceleration = kinetics->acceleration;
    
    float g_y = -0.000981;
    acceleration = simd::float3{0, g_y, 0};
    velocity += acceleration;
    position += velocity;
    
    matrix_float4x4 translationMatrix = matrix4x4_translation(position);

    // TODO: Introduce physics system
    float angleInDegrees = glfwGetTime()/2.0 * 90;
    float angleInRadians = angleInDegrees * M_PI / 180.0f;
    matrix_float4x4 rotationMatrix = matrix4x4_rotation(angleInRadians, 1.0, 1.0, 0.0);
    matrix_float4x4 scaleMatrix = matrix4x4_scale(transform->scale);

    matrix_float4x4 modelMatrix = simd_mul(translationMatrix, scaleMatrix);
    modelMatrix = simd_mul(modelMatrix, rotationMatrix);

    matrix_float4x4 viewMatrix = camera->generate_view_matrix();
    
    float aspectRatio = (layer->drawableSize().width / layer->drawableSize().height);

    matrix_float4x4 perspectiveMatrix = matrix_perspective_right_hand(camera->fov, aspectRatio, camera->nearZ, camera->farZ);

    TransformationData transformationData = { modelMatrix, viewMatrix, perspectiveMatrix };
    memcpy(transformationBuffer->contents(), &transformationData, sizeof(transformationData));

    renderCommandEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
    renderCommandEncoder->setCullMode(MTL::CullModeBack);
//    renderCommandEncoder->setTriangleFillMode(MTL::TriangleFillModeLines);
    renderCommandEncoder->setRenderPipelineState(metalRenderPSO);
    renderCommandEncoder->setDepthStencilState(depthStencilState);
    renderCommandEncoder->setVertexBuffer(cubeVertexBuffer, 0, 0);
    renderCommandEncoder->setVertexBuffer(transformationBuffer, 0, 1);
    MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
    NS::UInteger vertexStart = 0;
    NS::UInteger vertexCount = 36;
    renderCommandEncoder->setFragmentTexture(anyaTexture->texture, 0);
    renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
}

void MTLEngine::frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    MTLEngine* engine = (MTLEngine*)glfwGetWindowUserPointer(window);
    engine->resizeFrameBuffer(width, height);
}

void MTLEngine::resizeFrameBuffer(int width, int height) {
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

void MTLEngine::createBuffers() {
    transformationBuffer = metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared);
}

void MTLEngine::createDepthAndMSAATextures() {
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

void MTLEngine::createRenderPassDescriptor() {
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

void MTLEngine::updateRenderPassDescriptor() {
    MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
    MTL::RenderPassDepthAttachmentDescriptor* depthAttachment = renderPassDescriptor->depthAttachment();
    
    colorAttachment->setTexture(msaaRenderTargetTexture);
    colorAttachment->setResolveTexture(metalDrawable->texture());
    
    depthAttachment->setTexture(depthTexture);
}
