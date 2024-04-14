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

int ComponentManager::register_geometry(int entityId, MTL::Buffer* data) {
    geometryData.insert({entityId, data});
    return entityId;
}

int ComponentManager::register_texture(int entityId, Texture* texture) {
    textureData.insert({entityId, texture});
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

MTL::Buffer* ComponentManager::get_geometry(int entityId) {
    if (!geometryData.contains(entityId)) {
        return nullptr;
    }
    
    return geometryData[entityId];
}

Texture* ComponentManager::get_texture(int entityId) {
    if (!textureData.contains(entityId)) {
        return nullptr;
    }
    
    return textureData[entityId];
}

void ComponentManager::cleanup() {
    // Should component manager be responsible for cleaning up the textures? Hmm..
    for (auto it = textureData.begin(); it != textureData.end(); ++it) {
        delete it->second;
    }
    textureData.clear();
    for (auto it = geometryData.begin(); it != geometryData.end(); ++it) {
        // TODO:
//        delete it->second;
    }
    geometryData.clear();
}
