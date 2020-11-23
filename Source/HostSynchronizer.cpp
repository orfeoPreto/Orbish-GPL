//
//  HostSynchronizer.cpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 18/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#include "HostSynchronizer.h"

HostSynchronizer::HostSynchronizer(OrbishContext* context): Synchronizer(context){
    
}
HostSynchronizer::~HostSynchronizer(){
    
}

int HostSynchronizer::getNextSample(SnapMode snapMode){
    double currentPos = Synchronizer::context->info->ppqPosition;
    double expectedPos = 0;
      // expected position is next down beat (except if buffer is exactly on beat
      if (snapMode == kSnapMeasure)
      {
          expectedPos = Synchronizer::context->info->ppqPositionOfLastBarStart;
          while (expectedPos < currentPos)
              expectedPos += double(Synchronizer::context->timeSigTop) / double(Synchronizer::context->timeSigBottom) * 4.0;
      }
      // snap to next quarter note
      else if (snapMode == kSnapQuarter)
      {
          expectedPos = floor(currentPos);
          if (expectedPos != currentPos)
              expectedPos++;
      }
      else if (snapMode == kSnapNone)
      {
          expectedPos = currentPos;
      }
      
      double expectedSamplePosition = Synchronizer::context->beatsToSamples(expectedPos);
      // nextSample is the expected sample position starting from the start of the current buffer
      int nextSample = int(floor(expectedSamplePosition + .5)) - int(Synchronizer::context->beatsToSamples(currentPos));
    return nextSample;
}
