//
//  Layer.cpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 19/02/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#include "Layer.h"

Layer::Layer(){}

Layer::Layer(int channels, int size){
    Buffer = std::make_shared<AudioBuffer<float> >();
    Buffer->clear();
    Buffer->setSize(channels, size);
}

Layer::~Layer(){
    
    
}
