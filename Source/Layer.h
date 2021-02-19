//
//  Layer.hpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 19/02/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef Layer_hpp
#define Layer_hpp

#include <stdio.h>
#include "JuceHeader.h"
#include <iostream>

class Layer
{
public:
    Layer();
    Layer(int, int);
    ~Layer();

    std::shared_ptr<AudioBuffer<float> > Buffer{};
    int Checkpoint = -1;
    bool dirty = false;
    int index = 0;
    bool FirstLayerBuffer = false;
    bool LastLayerBuffer = false;
};

#endif /* Layer_hpp */
