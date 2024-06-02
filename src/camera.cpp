//
//  camera.cpp
//  MetalTest
//
//  Created by Mac on 4/11/24.
//

#include "camera.hpp"
#include "AAPLMathUtilities.h"
#include <iostream>

Camera::Camera(simd::float3 position) : position(position) {}

void Camera::update() {
    simd::float3 tmp_front;
    std::cout << yaw << " " << pitch << std::endl;
    tmp_front.x = cos(radians_from_degrees(yaw)) * cos(radians_from_degrees(pitch));
    tmp_front.y = sin(radians_from_degrees(pitch));
    tmp_front.z = sin(radians_from_degrees(yaw)) * cos(radians_from_degrees(pitch));
    front = simd::normalize(tmp_front);
}

void Camera::updateFrontFromScreenCoords(GLFWwindow* window, double xpos, double ypos) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->handleCursorPositionChange(xpos, ypos);
    }
}

void Camera::handleCursorPositionChange(double xpos, double ypos) {
    double dx = (xpos - lastX) * 0.1f;
    double dy = (ypos - lastY) * 0.1f;
    
    yaw -= dx;
    pitch -= dy;
    lastX = xpos;
    lastY = ypos;
    update();
}

matrix_float4x4 Camera::generate_view_matrix() const {
    simd::float3 worldUp = simd::float3{0.0f, 1.0f, 0.0f};
    simd::float3 right = simd::normalize(simd::cross(worldUp, front));
    simd::float3 up = simd::cross(front, right);
    return matrix_make_rows(right.x,    right.y,    right.z,  simd::dot(-right, position),
                            up.x,       up.y,       up.z,     simd::dot(-up, position),
                            -front.x,    -front.y,    -front.z,  simd::dot(front, position),
                            0, 0, 0, 1);
}
