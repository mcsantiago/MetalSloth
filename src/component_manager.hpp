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
public:
    int register_transform(int entityId, Transform transformComponent);
    int register_kinetic_physical_properties(int entityId, KineticPhysicalProperties component);
    
    Transform* get_transform(int entityId);
    KineticPhysicalProperties* get_kinetics(int entityId);
};

#endif /* component_manager_hpp */
