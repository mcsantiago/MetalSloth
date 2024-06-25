//
//  mesh_component.hpp
//  MetalTest
//
//  Created by Mac on 6/19/24.
//

#ifndef mesh_component_hpp
#define mesh_component_hpp

#include <iostream>
#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "ufbx/ufbx.h"

struct MeshInfo {
    unsigned int id = 0;
    unsigned long numTriangles = 0;
    unsigned long numVerteces = 0;
    MTL::Buffer* vertexBuffer;
};

#endif /* mesh_component_hpp */
