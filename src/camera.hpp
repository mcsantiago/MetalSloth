//
//  camera.hpp
//  MetalTest
//
//  Created by Mac on 4/11/24.
//

#ifndef camera_hpp
#define camera_hpp

#define GLFW_INCLUDE_NONE
#import <GLFW/glfw3.h>

#include "glfw_bridge.hpp"
#include <simd/simd.h>

class Camera {
public:
    simd::float3 position;
    simd::float3 front;
    simd::float3 right;

    // Camera options
    float cameraSpeed = 50.0f;
    float mouseSensitivity = 1.0f;
    
    // TODO: encapsulate these properly
    float fov = 90 * (M_PI / 180.0f);
    float nearZ = 0.1f;
    float farZ = 100.0f;
    
    // Euler angles
    float yaw;
    float pitch;
    double lastX = 400;
    double lastY = 300;
    
    Camera(simd::float3 position);
    
    void update();
    void handleCursorPositionChange(double xpos, double ypos);
    static void updateFrontFromScreenCoords(GLFWwindow* window, double xpos, double ypos);
    matrix_float4x4 generate_view_matrix() const;
};

#endif /* camera_hpp */
