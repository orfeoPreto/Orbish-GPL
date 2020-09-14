//
//  Realignment.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 13/09/2020.
//

#ifndef Realignment_h
#define Realignment_h

#include <stdio.h>
#include <limits.h>
#include "JuceHeader.h"

class Realignment{
public:
    Realignment();
    Realignment(int, int);
    ~Realignment();
    int getCurrentOffset ();
    int getAccumulatedOffset ();
    int getTotalOffset ();
    void setTotalOffset(int);
    bool isRealigned ();
    void setRealigned(bool);
    int getTotalBuffersToSpreadFade ();
    int getRemainingBuffersToSpreadFade ();
    int getProcessedBuffersToSpreadFade ();
    float getCurrentFadeLevelStart();
    float getCurrentFadeLevelEnd();
    bool isSyncInProgress ();
    bool isFadeInProgress ();
    void BufferProcessed();
    void setSampleRate(int);
    void setBlockSize(int);
    
private:
    int block = 0;
    int currentOffset = 0;
    int totalOffset = 0;
    int accumulatedOffset = 0;
    int totalBuffersToSpreadFade=0;
    int remainingBuffersToSpreadFade=0;
    int processedBuffersToSpreadFade=0;
    float fadeLevelDiff = 0;
    float currentFadeLevelStart = 0;
    float currentFadeLevelEnd = 0;
    int smplRate;
    bool synced = false;
    bool syncInProgress = false;
    bool fadeInProgress = false;
};

#endif /* Realignment_h */
