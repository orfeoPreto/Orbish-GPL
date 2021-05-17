//
//  GainModifier.cpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 14/09/2020.
//

#include "GainModifier.h"


GainModifier::GainModifier(){}
GainModifier::~GainModifier(){
    buffer = nullptr;
}

void GainModifier::applyGain(){
    if(startIndex + numberOfSamples > buffer->getNumSamples())return;
    switch (operation) {
        case OperationType::ChannelRegion:
           for (auto channel = 0; channel<buffer->getNumChannels();++channel){
               buffer->applyGain(channel, startIndex, numberOfSamples, startLevel);
           }
            break;
        case OperationType::Region:
            buffer->applyGain(startIndex, numberOfSamples, startLevel);
            break;
        case OperationType::All:
            buffer->applyGain(startLevel);
            break;
        case OperationType::RampChannelRegion:
            for (auto channel = 0; channel<buffer->getNumChannels();++channel){
                buffer->applyGainRamp(channel, startIndex, numberOfSamples, startLevel, endLevel);
            }
            break;
        case OperationType::RampRegion:
            buffer->applyGainRamp(startIndex, numberOfSamples, startLevel, endLevel);
            break;
        default:
            break;
    }
}


