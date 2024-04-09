//
//  main.cpp
//  MetalTest
//
//  Created by Mac on 4/7/24.
//

#include <iostream>
#include "mtl_engine.hpp"


int main(int argc, const char * argv[]) {
    MTLEngine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    return 0;
}
