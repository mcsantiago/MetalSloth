//
//  vertex_data.h
//  MetalTest
//
//  Created by Mac on 4/9/24.
//

#ifndef vertex_data_h
#define vertex_data_h

#include <simd/simd.h>

using namespace simd;

struct VertexData {
    float4 position;
    float3 normal;
    float2 textureCoordinate;
};

struct TransformationData {
    float4x4 modelMatrix;
    float4x4 viewMatrix;
    float4x4 perspectiveMatrix;
};

#endif /* vertex_data_h */
