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

Transform* ComponentManager::get_transform(int entityId) {
    if (!transforms.contains(entityId)) {
        return nullptr;
    }
    
    return &transforms[entityId];
}
