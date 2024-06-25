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

int ComponentManager::register_geometry(int entityId, MeshInfo data) {
    geometryData.insert({entityId, data});
    return entityId;
}

int ComponentManager::register_texture(int entityId, Texture texture) {
    textureData.insert({entityId, texture});
    return entityId;
}

std::optional<Transform> ComponentManager::get_transform(int entityId) {
    if (!transforms.contains(entityId)) {
        return std::nullopt;
    }
    
    return std::optional<Transform>{transforms[entityId]};
}

std::optional<KineticPhysicalProperties> ComponentManager::get_kinetics(int entityId) {
    if (!kinetics.contains(entityId)) {
        return std::nullopt;
    }
    
    return std::optional<KineticPhysicalProperties>{kinetics[entityId]};
}

std::optional<MeshInfo> ComponentManager::get_geometry(int entityId) {
    if (!geometryData.contains(entityId)) {
        return std::nullopt;
    }
    
    return std::optional<MeshInfo>{geometryData[entityId]};
}

std::optional<Texture> ComponentManager::get_texture(int entityId) {
    if (!textureData.contains(entityId)) {
        return std::nullopt;
    }
    
    return std::optional<Texture>{textureData[entityId]};
}

void ComponentManager::cleanup() {
    // Should component manager be responsible for cleaning up the textures? Hmm..
    for (auto it = textureData.begin(); it != textureData.end(); ++it) {
        free(it->second.texture);
    }
    textureData.clear();
    for (auto it = geometryData.begin(); it != geometryData.end(); ++it) {
        free(it->second.vertexBuffer);
    }
    geometryData.clear();
}
