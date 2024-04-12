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
    simd::float3 front;
    simd::float3 up;
    simd::float3 right;
    simd::float3 worldUp;
    
    Camera(simd::float3 position = simd::float3{0.0f, 0.0f,  0.0f},
           simd::float3 up       = simd::float3{0.0f, 1.0f,  0.0f},
           simd::float3 right    = simd::float3{1.0f, 0.0f,  0.0f},
           simd::float3 front    = simd::float3{0.0f, 0.0f, -1.0f});
    
    matrix_float4x4 generate_view_matrix() const;
};

#endif /* camera_hpp */
