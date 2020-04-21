//
//  Visualiser.hpp
//  MofflR - VST3
//
//  Created by Duke Quarcoo on 28/04/2019.
//  Copyright © 2019 EXU. All rights reserved.
//
#pragma once

#ifndef Visualiser_h
#define Visualiser_h

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"

class Visualiser: public AudioVisualiserComponent
{
private:
    
public:
    Visualiser();
    Visualiser(int bufferSize, int samplesPerBlock, int channels);
    ~Visualiser();
};

#endif /* Visualiser_hpp */
