//
//  Visualiser.cpp
//  MofflR - VST3
//
//  Created by Duke Quarcoo on 28/04/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "Visualiser.h"

Visualiser::Visualiser() :  AudioVisualiserComponent(2)
{
}

Visualiser::Visualiser(int bufferSize, int samplesPerBlock, int channels) : AudioVisualiserComponent(channels)
{
    setBufferSize(bufferSize);
    setSamplesPerBlock(samplesPerBlock);
    setColours(Colours::black, Colours::dodgerblue);
}

Visualiser::~Visualiser(){
    int i = 0;
}
