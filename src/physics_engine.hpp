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

class PhysicsSystem {
private:
    ComponentManager* manager;
    MTL::Device* metalDevice;
    const float G = 6.674e-11;          // 6.674e-11 (Nm^2)/(kg^2)
    const float g = 9.81;               // 9.81 m/(s^2)
    const float physical_scale = 1e-4;  // universal scale factor to look nice on screen
    
public:
    PhysicsSystem(ComponentManager* manager, MTL::Device* metalDevice) {
        this->manager = manager;
        this->metalDevice = metalDevice;
    };
    
    void update_loaded_entities(float deltaTime) {
        for (int i = 0; i < manager->getNumEntities(); i++) {
            update_entity(i, deltaTime);
        }
    }
    
    void update_entity(int entityId, float deltaTime) {
        std::optional<Transform> transform = manager->get_transform(entityId);
        std::optional<KineticPhysicalProperties> kinetics = manager->get_kinetics(entityId);
        
        if (!transform || !kinetics) {
            return;
        }
        
        simd::float3 position = transform->position;
        simd::float3 velocity = kinetics->velocity;
        simd::float3 acceleration = kinetics->acceleration;
        
        float g_y = -1 * g * physical_scale;
        acceleration = simd::float3{0, g_y, 0};
        velocity += acceleration;
        position += velocity;
    }
    
    void cleanup() {
        // manager will be cleaned up by SlothEngine
    }
};

#endif /* physics_engine_hpp */
