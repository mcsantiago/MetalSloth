//
//  component_manager.cpp
//  MetalTest
//
//  Created by Mac on 4/12/24.
//

#include "component_manager.hpp"

int ComponentManager::register_transform(int entityId, Transform transformComponent) {
    transforms.insert({entityId, transformComponent});
    return entityId;
}

int ComponentManager::register_kinetic_physical_properties(int entityId, KineticPhysicalProperties component) {
    kinetics.insert({entityId, component});
    return entityId;
}

Transform* ComponentManager::get_transform(int entityId) {
    if (!transforms.contains(entityId)) {
        return nullptr;
    }
    
    return &transforms[entityId];
}

KineticPhysicalProperties* ComponentManager::get_kinetics(int entityId) {
    if (!kinetics.contains(entityId)) {
        return nullptr;
    }
    
    return &kinetics[entityId];
}
