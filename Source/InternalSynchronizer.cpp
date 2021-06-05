/*
  ==============================================================================

    InternalSynchronizer.cpp
    Created: 18 Nov 2020 11:21:03pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include "InternalSynchronizer.h"

InternalSynchronizer::InternalSynchronizer(std::shared_ptr<OrbishContext> context, int* pos): Synchronizer(context),currentPos(pos){
}
InternalSynchronizer::~InternalSynchronizer(){
    
}

int InternalSynchronizer::getNextSample(SnapMode snapMode){
    int expectedPos = 0;
      // expected position is next down beat (except if buffer is exactly on beat
      if (snapMode == kSnapMeasure)
      {
#if DEBUG_LOG
          context->logMessage(String(*currentPos));
#endif
         int diff = *currentPos % (Synchronizer::context->samplesPerBeat  * Synchronizer::context->timeSigTop);
          expectedPos = *currentPos - diff;
          while (expectedPos < *currentPos)
              expectedPos += Synchronizer::context->timeSigTop * Synchronizer::context->samplesPerBeat;
      }
      // snap to next quarter note
      else if (snapMode == kSnapQuarter)
      {
          int diff = *currentPos % (Synchronizer::context->samplesPerBeat);
           expectedPos = *currentPos - diff;
          if (expectedPos != *currentPos)
              expectedPos+=Synchronizer::context->samplesPerBeat;
      }
      else if (snapMode == kSnapNone)
      {
          expectedPos = *currentPos;
      }
    return expectedPos - *currentPos;
}

void InternalSynchronizer::setCurrentPosition(int *currentPosition){
    currentPos = currentPosition;
}


