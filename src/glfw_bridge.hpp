//
//  glfw_bridge.hpp
//  MetalTest
//
//  Created by Mac on 4/9/24.
//

#ifndef glfw_bridge_hpp
#define glfw_bridge_hpp
struct GLFWwindow;

namespace CA {
class MetalLayer;
}

namespace GLFWBridge {
void AddLayerToWindow(GLFWwindow* window, CA::MetalLayer* layer);
}


#endif /* glfw_bridge_hpp */
