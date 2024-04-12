//
//  camera.cpp
//  MetalTest
//
//  Created by Mac on 4/11/24.
//

#include "camera.hpp"
#include "AAPLMathUtilities.h"

Camera::Camera(simd::float3 position, simd::float3 up, simd::float3 right, simd::float3 front) 
    : position(position), up(up), right(right), front(front) {}

matrix_float4x4 Camera::generate_view_matrix() const {
    return matrix_make_rows(right.x,    right.y,    right.z,  simd::dot(-right, position),
                            up.x,       up.y,       up.z,     simd::dot(-up, position),
                            -front.x,   -front.y,   -front.z, simd::dot(front, position),
                            0, 0, 0, 1);
}
