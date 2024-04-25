//
//  camera.cpp
//  MetalTest
//
//  Created by Mac on 4/11/24.
//

#include "camera.hpp"
#include "AAPLMathUtilities.h"

Camera::Camera(simd::float3 position, simd::float3 target)
    : position(position), target(target) {}

matrix_float4x4 Camera::generate_view_matrix() const {
    simd::float3 worldUp = simd::float3{0.0f, 1.0f, 0.0f};
    simd::float3 front = simd::normalize(target-position);
    simd::float3 right = simd::normalize(simd::cross(worldUp, front));
    simd::float3 up = simd::cross(front, right);
    return matrix_make_rows(right.x,    right.y,    right.z,  simd::dot(-right, position),
                            up.x,       up.y,       up.z,     simd::dot(-up, position),
                            -front.x,    -front.y,    -front.z,  simd::dot(front, position),
                            0, 0, 0, 1);
}
