//
//  component_manager.hpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#ifndef component_manager_hpp
#define component_manager_hpp

#include <simd/simd.h>
#include <unordered_map>
#include <vector>
#include "vertex_data.h"
#include "texture.hpp"

struct Transform {
    simd::float3 position;
    simd::float3 rotation;
    simd::float3 scale;
};

struct KineticPhysicalProperties {
    float mass;
    simd::float3 velocity;
    simd::float3 acceleration;
};

class ComponentManager {
private:
    std::unordered_map<int, Transform> transforms;
    std::unordered_map<int, KineticPhysicalProperties> kinetics;
    std::unordered_map<int, std::vector<MTL::Buffer*>> geometryData;
    std::unordered_map<int, Texture*> textureData;
public:
    int register_transform(int entityId, Transform);
    int register_kinetic_physical_properties(int entityId, KineticPhysicalProperties);
    int register_geometry(int entityId, MTL::Buffer* geometryBuffer);
    int register_texture(int entityId, Texture* texture);
    
    int getNumEntities() {
        return 1;
    }
    void cleanup();

    Transform* get_transform(int entityId);
    KineticPhysicalProperties* get_kinetics(int entityId);
    MTL::Buffer* get_geometry(int entityId);
    Texture* get_texture(int entityId);
};

#endif /* component_manager_hpp */
