//
//  physics_engine.hpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#ifndef physics_engine_hpp
#define physics_engine_hpp

#include <simd/simd.h>
#include "component_manager.hpp"

namespace phys {
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
}

#endif /* physics_engine_hpp */
