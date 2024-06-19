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
    initImGui();
    initSystems();
    loadScene();
    initGlfwCallbacks();
}

void SlothEngine::initDevice() {
    metalDevice = MTL::CreateSystemDefaultDevice();
}

void SlothEngine::initSystems() {
    componentManager = new ComponentManager();
    renderingSystem = new MTLRenderingSystem(componentManager, metalDevice, glfwWindow);
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

void SlothEngine::initImGui() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplMetal_Init(metalDevice);
}

void SlothEngine::initGlfwCallbacks() {
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(glfwWindow, camera);
    glfwSetCursorPosCallback(glfwWindow, camera->updateFrontFromScreenCoords);
}

void SlothEngine::createCamera() {
    int c = cbrt(static_cast<double>(componentManager->getNumEntities()));
    simd::float3 position   = simd::float3{c * 1.0f, c * 1.0f,  c * 3.0f};
    camera = new Camera(position);
}

void SlothEngine::loadScene() {
    createCamera();
//    texture = new Texture("assets/anya.jpg", metalDevice);
    loadFbxFile("assets/X Bot.fbx");
    loadObject(0,
               {0.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, 0.0f},
               {1.0f, 1.0f, 1.0f},
               {0.0f, 0.0f, 0.0f},
               {0.0f, 0.0f, 0.0f});
}

void SlothEngine::loadFbxFile(const char *filename) {
    ufbx_error error;
    
    ufbx_scene* scene = ufbx_load_file(filename, NULL, &error);
    if (!scene) {
        fprintf(stderr, "Failed to load: %s\n", error.description.data);
        exit(1);
    }
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node *node = scene->nodes.data[i];
        if (node->is_root) continue;

        printf("Object: %s\n", node->name.data);
        if (node->mesh) {
            loadPolygons(0, node->mesh);
            printf("-> mesh with %zu faces\n", node->mesh->faces.count);
        }
    }
    for (size_t i = 0; i < scene->materials.count; i++) {
        ufbx_material *material = scene->materials.data[i];
        std::cout << "Material: " << material->name.data << std::endl;
        std::cout << "Texture count: " << material->textures.count << std::endl;

        for (size_t j = 0; j < material->textures.count; ++j) {
            ufbx_texture *texture = material->textures.data[j].texture;
            if (texture && texture->filename.length > 0) {
                std::cout << "  Texture: " << texture->filename.data << std::endl;
            }
        }
    }
    ufbx_free_scene(scene);
}

void SlothEngine::loadPolygons(int entityId, ufbx_mesh *mesh) {
    // The vertex buffer is allocated in heap, the component manager should release it.
    // Maybe an atomic reference counter would be necessary, but for now I will only free memory
    // once the component_manager is freed.
    
    size_t n = mesh->faces.count;
    std::vector<VertexData> triangles;
    for (size_t i = 0; i < n; i++) {
        ufbx_face face = mesh->faces.data[i];
        
        if (face.num_indices == 3) {
            for (uint32_t corner = 0; corner < face.num_indices; corner++) {
                uint32_t index = face.index_begin + corner;
                ufbx_vec3 position = mesh->vertex_position[index];
                ufbx_vec3 normal = mesh->vertex_normal[index];
                ufbx_vec2 uv = mesh->vertex_uv[index];
                
                VertexData point = {
                    {(float)position.x, (float)position.y, (float)position.z, 1.0f},
                    {(float)normal.x, (float)normal.y, (float)normal.z},
                    {(float)uv.x, (float)uv.y}
                };
                triangles.push_back(point);
            }
        } else if (face.num_indices == 4) {
            // TODO: I'm sure there's a nice way to write this function.
            uint32_t offset = face.index_begin;
            ufbx_vec3 position0 = mesh->vertex_position[offset];
            ufbx_vec3 normal0 = mesh->vertex_normal[offset];
            ufbx_vec2 uv0 = mesh->vertex_uv[offset];
            
            ufbx_vec3 position1 = mesh->vertex_position[offset + 1];
            ufbx_vec3 normal1 = mesh->vertex_normal[offset + 1];
            ufbx_vec2 uv1 = mesh->vertex_uv[offset + 1];
            
            ufbx_vec3 position2 = mesh->vertex_position[offset + 2];
            ufbx_vec3 normal2 = mesh->vertex_normal[offset + 2];
            ufbx_vec2 uv2 = mesh->vertex_uv[offset + 2];
            
            ufbx_vec3 position3 = mesh->vertex_position[offset + 3];
            ufbx_vec3 normal3 = mesh->vertex_normal[offset + 3];
            ufbx_vec2 uv3 = mesh->vertex_uv[offset + 3];

            triangles.push_back({
                {(float)position0.x, (float)position0.y, (float)position0.z, 1.0f},
                {(float)normal0.x, (float)normal0.y, (float)normal0.z},
                {(float)uv0.x, (float)uv0.y}
            });
            triangles.push_back({
                {(float)position1.x, (float)position1.y, (float)position1.z, 1.0f},
                {(float)normal1.x, (float)normal1.y, (float)normal1.z},
                {(float)uv1.x, (float)uv1.y}
            });
            triangles.push_back({
                {(float)position2.x, (float)position2.y, (float)position2.z, 1.0f},
                {(float)normal2.x, (float)normal2.y, (float)normal2.z},
                {(float)uv2.x, (float)uv2.y}
            });
            
            triangles.push_back({
                {(float)position0.x, (float)position0.y, (float)position0.z, 1.0f},
                {(float)normal0.x, (float)normal0.y, (float)normal0.z},
                {(float)uv0.x, (float)uv0.y}
            });
            triangles.push_back({
                {(float)position2.x, (float)position2.y, (float)position2.z, 1.0f},
                {(float)normal2.x, (float)normal2.y, (float)normal2.z},
                {(float)uv2.x, (float)uv2.y}
            });
            triangles.push_back({
                {(float)position3.x, (float)position3.y, (float)position3.z, 1.0f},
                {(float)normal3.x, (float)normal3.y, (float)normal3.z},
                {(float)uv3.x, (float)uv3.y}
            });
        }
    }
    vertexBuffer = metalDevice->newBuffer(triangles.data(),
                                          sizeof(VertexData) * triangles.size(),
                                          MTL::ResourceStorageModeShared);

    componentManager->register_geometry(entityId, vertexBuffer);
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
    
//    transformationBuffers.insert({entityId, metalDevice->newBuffer(sizeof(TransformationData), MTL::ResourceStorageModeShared)});
}

void SlothEngine::run() {
    while (!glfwWindowShouldClose(glfwWindow)) {
        std::chrono::time_point now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFrameTime);
        float deltaTime = duration.count() / 1000.0;
        // TODO: encapsulate this in to an event queue?
        glfwPollEvents();
        camera->update();
        if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS) {
            camera->position += camera->front * camera->cameraSpeed * deltaTime;
        }
        else if(glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS) {
            camera->position -= camera->front * camera->cameraSpeed * deltaTime;
        }
        else if(glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS) {
            simd::float3 right = simd::normalize(simd::cross(simd::float3{0.0f, 1.0f, 0.0f}, camera->front));
            camera->position -= right * camera->cameraSpeed * deltaTime;
        }
        else if(glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS) {
            simd::float3 right = simd::normalize(simd::cross(simd::float3{0,1,0}, camera->front));
            camera->position += right * camera->cameraSpeed * deltaTime;
        }
        else if(glfwGetKey(glfwWindow, GLFW_KEY_Q) == GLFW_PRESS) {
            if (renderMode == full) {
                renderMode = wireframe;
            } else {
                renderMode = full;
            }
        }
//        physicsSystem->update_loaded_entities(deltaTime);
        renderingSystem->run(camera, renderMode);
        lastFrameTime = now;
    }
}

void SlothEngine::cleanup() {
    renderingSystem->cleanup();
    physicsSystem->cleanup();
    metalDevice->release();
    
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    delete camera;
    delete componentManager;
    delete renderingSystem;
    delete physicsSystem;
//    delete texture;
}
