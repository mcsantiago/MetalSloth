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

class ComponentManager {
private:
    std::unordered_map<int, Transform> transforms;
public:
    int register_transform(int entityId, Transform transformComponent);
    Transform* get_transform(int entityId);
};

#endif /* component_manager_hpp */
