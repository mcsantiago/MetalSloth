//
//  triangle.metal
//  MetalTest
//
//  Created by Mac on 4/8/24.
//

#include <metal_stdlib>
#include <metal_common>
using namespace metal;

#include "../vertex_data.h"

struct VertexOut {
    // The [[position]] attribute of this member indicates that this value
    // is the clip space position of the vertex when this structure is
    // returned from the vertex function.
    float4 position [[position]];
    float3 normal [[normal]];
    
    float3 fragPos;

    // Since this member does not have a special attribute, the rasterizer
    // interpolates its value with the values of the other triangle vertices
    // and then passes the interpolated value to the fragment shader for each
    // fragment in the triangle.
    float2 textureCoordinate;
};


vertex VertexOut vertexShader(uint vertexID [[vertex_id]],
             constant VertexData* vertexData [[buffer(0)]],
             constant TransformationData* transformationData [[buffer(1)]],
             uint instanceId [[instance_id]]) {
    VertexOut out;
    out.position = transformationData[instanceId].perspectiveMatrix * transformationData[instanceId].viewMatrix * transformationData[instanceId].modelMatrix * vertexData[vertexID].position;
    out.fragPos = float3(transformationData[instanceId].modelMatrix * float4(vertexData[vertexID].position));
    out.normal = vertexData[vertexID].normal;
    out.textureCoordinate = vertexData[vertexID].textureCoordinate;
    return out;
}

fragment float4 fragmentShader(VertexOut in [[stage_in]],
                               texture2d<float> colorTexture [[texture(0)]]) {
    // scene setup
    float3 lightPos = {10, 0, 0};
    float3 lightColor = {0.1, 0.9, 0};
    float3 objectColor = {1, 0, 0};
    float ambientStrength = 0.5;

    float3 norm = normalize(in.normal);
    float3 lightDir = normalize(lightPos - in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * lightColor;
    
    float3 ambient = ambientStrength * lightColor;
    
    float3 result = diffuse + ambient + objectColor;
    result = clamp(result, 0, 1);
    return float4(result, 1);
    
}
