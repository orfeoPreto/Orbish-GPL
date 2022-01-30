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
    Buffer = nullptr;
}

void Layer::makeVisualizationBuffer(int size){
    if(sizeof(visualizationBuffer) > 0){
        FloatVectorOperations::clear(visualizationBuffer, BUFFER_READ_SIZE);
        for (auto i=0; i<BUFFER_READ_SIZE; ++i) {
            visualizationBuffer[i] =  Buffer->getSample(0, float(i)/float(BUFFER_READ_SIZE) * size);
        }
    }
}
