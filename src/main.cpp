//
//  main.cpp
//  MetalTest
//
//  Created by Mac on 4/7/24.
//

#include <iostream>
#include "sloth_engine.hpp"


int main(int argc, const char * argv[]) {
    SlothEngine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    return 0;
}
