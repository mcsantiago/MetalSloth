//
//  texture.hpp
//  MetalTest
//
//  Created by Mac on 4/9/24.
//

#ifndef texture_hpp
#define texture_hpp

#include <Metal/Metal.hpp>
#include <stb/stb_image.h>

class Texture {
public:
    Texture();
    Texture(const char* filepath, MTL::Device* metalDevice);
    ~Texture();
    MTL::Texture* texture;
    int width, height, channels;
};

#endif /* texture_hpp */
