//
//  HostSynchronizer.cpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 18/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#include "HostSynchronizer.h"

HostSynchronizer::HostSynchronizer(std::shared_ptr<OrbishContext> context): Synchronizer(context){
    
}
HostSynchronizer::~HostSynchronizer(){
    
}

int HostSynchronizer::getNextSynchronizationPoint(SnapMode snapMode){
    double currentPos = Synchronizer::context->info->ppqPosition;
    double expectedPos = 0;
    switch(snapMode){
        // expected position is next down beat (except if buffer start is exactly on beat
        case kSnapMeasure:
        case kSnapLoop:{
            expectedPos = Synchronizer::context->info->ppqPositionOfLastBarStart;
            while (expectedPos < currentPos)
                expectedPos += double(Synchronizer::context->timeSigTop) / double(Synchronizer::context->timeSigBottom) * 4.0;
            break;
        }
        // snap to next quarter note
        case kSnapQuarter:{
            expectedPos = floor(currentPos);
            if (expectedPos != currentPos)
                expectedPos++;
            break;
        }
        case kSnapNone:{
            expectedPos = currentPos;
            break;
        }
        case kSnapHostLoop:{
            if (Synchronizer::context->info->isLooping) {
                expectedPos = Synchronizer::context->info->ppqLoopEnd;
                if(currentPos == Synchronizer::context->info->ppqLoopStart){
                    currentPos = Synchronizer::context->info->ppqLoopEnd;
                }
            }else{
                expectedPos = currentPos;
            }
            break;
        }
    }
    
      double expectedSamplePosition = Synchronizer::context->beatsToSamples(expectedPos);
      // nextSample is the expected sample position starting from the start of the current buffer
      int nextSample = int(floor(expectedSamplePosition + .5)) - int(Synchronizer::context->beatsToSamples(currentPos));
        return nextSample;
}
