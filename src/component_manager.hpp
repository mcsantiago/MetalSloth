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
#include <unordered_set>
#include <vector>
#include <optional>
#include "vertex_data.h"
#include "texture.hpp"
#include "component_types/mesh_component.hpp"


struct KineticPhysicalProperties {
    float mass;
    simd::float3 velocity;
    simd::float3 acceleration;
};

class ComponentManager {
private:
    std::unordered_set<int> entities;
    std::unordered_map<int, Transform> transforms;
    std::unordered_map<int, KineticPhysicalProperties> kinetics;
    std::unordered_map<int, MeshInfo> geometryData;
    std::unordered_map<int, Texture> textureData;
public:
    int register_transform(int entityId, Transform);
    int register_kinetic_physical_properties(int entityId, KineticPhysicalProperties);
    int register_geometry(int entityId, MeshInfo mesh);
    int register_texture(int entityId, Texture texture);
    
    size_t getNumEntities() {
        return entities.size();
    }
    void cleanup();

    std::optional<Transform> get_transform(int entityId);
    std::optional<KineticPhysicalProperties> get_kinetics(int entityId);
    std::optional<MeshInfo> get_geometry(int entityId);
    std::optional<Texture> get_texture(int entityId);
};

#endif /* component_manager_hpp */
