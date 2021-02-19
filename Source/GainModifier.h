//
//  GainModifier.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 14/09/2020.
//

#ifndef GainModifier_h
#define GainModifier_h
#include <JuceHeader.h>
#include <stdio.h>



class GainModifier{

    
public:
    enum OperationType{
        ChannelRegion = 0,
        Region,
        All,
        RampChannelRegion,
        RampRegion
    };
    GainModifier();
    ~GainModifier();
    void applyGain();
    
    std::shared_ptr<AudioBuffer<float> > buffer;
    int startIndex=0;
    int numberOfSamples=0;
    float startLevel=0;
    float endLevel=0;
    GainModifier::OperationType operation;

};
#endif /* GainModifier_h */
