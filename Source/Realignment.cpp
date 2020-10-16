//
//  Realignment.cpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 13/09/2020.
//

#include "Realignment.h"

Realignment::Realignment(){
    
}
Realignment::Realignment(int samplesPerBlock, int sampleRate){
    block = samplesPerBlock;
    smplRate = sampleRate;
}
Realignment::~Realignment(){
    
}
int Realignment::getCurrentOffset (){
    return currentOffset;
}
int Realignment::getTotalOffset (){
    return totalOffset;
}
int Realignment::getAccumulatedOffset (){
    return accumulatedOffset;
}
void Realignment::setTotalOffset(int offset){
    totalOffset = offset;
    auto sign = 1;
    if(offset != 0){
        offset = std::min(abs(offset),block);
        sign = offset/abs(offset);
        syncInProgress = true;
        fadeInProgress = true;
    }else{
        accumulatedOffset = 0;
    }
    currentOffset = offset * sign;
    int fadeOffset = std::min(offset, int(smplRate * .01));
    remainingBuffersToSpreadFade = totalBuffersToSpreadFade = fadeOffset / block;
    if (totalBuffersToSpreadFade == 0 && fadeOffset%block > 0) {
            remainingBuffersToSpreadFade = totalBuffersToSpreadFade = 1;
    }
    processedBuffersToSpreadFade = 0;
    fadeLevelDiff = totalBuffersToSpreadFade == 0 ? 0: 1/totalBuffersToSpreadFade;
    currentFadeLevelStart = 0;
    currentFadeLevelEnd  = (processedBuffersToSpreadFade + 1)*fadeLevelDiff;
}
int Realignment::getTotalBuffersToSpreadFade (){
    return totalBuffersToSpreadFade;
}
int Realignment::getRemainingBuffersToSpreadFade (){
    return remainingBuffersToSpreadFade;
}
int Realignment::getProcessedBuffersToSpreadFade (){
    return processedBuffersToSpreadFade;
}
float Realignment::getCurrentFadeLevelStart (){
    return currentFadeLevelStart;
}
float Realignment::getCurrentFadeLevelEnd (){
    return currentFadeLevelEnd;
}
bool Realignment::isSyncInProgress (){
    return syncInProgress;
}
bool Realignment::isFadeInProgress (){
    return fadeInProgress;
}
bool Realignment::isRealigned (){
    return synced;
}
void Realignment::setBlockSize(int blockSize){
    block = blockSize;
}
void Realignment::setSampleRate(int sampleRate){
    smplRate = sampleRate;
}
void Realignment::setRealigned(bool realigned){
    synced = realigned;
    if(synced){
        setTotalOffset(0);
        syncInProgress = false;
        fadeInProgress = false;
        processedBuffersToSpreadFade = totalBuffersToSpreadFade = 0;
        currentFadeLevelEnd = 0;
        currentFadeLevelStart = 0;
    }
}
void Realignment::BufferProcessed(){
    accumulatedOffset += currentOffset;
    if(--remainingBuffersToSpreadFade <= 0){
        fadeInProgress = false;
        setRealigned(true);
    }

    processedBuffersToSpreadFade = totalBuffersToSpreadFade - remainingBuffersToSpreadFade;
    currentFadeLevelStart = processedBuffersToSpreadFade * fadeLevelDiff;
    currentFadeLevelEnd = (processedBuffersToSpreadFade + 1)*fadeLevelDiff;
    currentOffset = std::min(totalOffset - accumulatedOffset, block);
}
