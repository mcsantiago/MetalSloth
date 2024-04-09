//
//  triangle.metal
//  MetalTest
//
//  Created by Mac on 4/8/24.
//

#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float4 color; // Add a color output
};

vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
                              constant simd::float3* vertexPositions) {
    VertexOut out;
    out.position = float4(vertexPositions[vertexID][0],
                          vertexPositions[vertexID][1],
                          vertexPositions[vertexID][2],
                          1.0);
    // Assign different colors to vertices for debugging
    if (vertexID == 0) {
        out.color = float4(1, 0, 0, 1); // Red
    } else if (vertexID == 1) {
        out.color = float4(0, 1, 0, 1); // Green
    } else {
        out.color = float4(0, 0, 1, 1); // Blue
    }
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]]) {
    // Use the interpolated color directly
    return in.color;
}
