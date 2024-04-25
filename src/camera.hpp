//
//  camera.hpp
//  MetalTest
//
//  Created by Mac on 4/11/24.
//

#ifndef camera_hpp
#define camera_hpp

#include <simd/simd.h>

class Camera {
public:
    simd::float3 position;
    simd::float3 target;
    
    // TODO: encapsulate these properly
    float fov = 90 * (M_PI / 180.0f);
    float nearZ = 0.1f;
    float farZ = 100.0f;
    
    simd::float3 worldUp;
    
    Camera(simd::float3 position,
           simd::float3 target);
    
    matrix_float4x4 generate_view_matrix() const;
};

#endif /* camera_hpp */
