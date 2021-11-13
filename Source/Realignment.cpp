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
int Realignment::getMinOffset(int offset){
    if(offset != 0){
        auto sign = 1;
        sign = offset/abs(offset);
        offset = std::min(abs(offset),block) * sign;
    }
    return offset;
}

void Realignment::setTotalOffset(int offset){
    totalOffset = offset;
    if(offset != 0){
        offset = getMinOffset(offset);
        syncInProgress = true;
        fadeInProgress = true;
    }else{
        accumulatedOffset = 0;
    }
    currentOffset = offset;
    int fadeOffset = std::min(abs(offset), int(smplRate * .01));
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
        setRealigned(true);
    }

    processedBuffersToSpreadFade = totalBuffersToSpreadFade - remainingBuffersToSpreadFade;
    currentFadeLevelStart = processedBuffersToSpreadFade * fadeLevelDiff;
    currentFadeLevelEnd = (processedBuffersToSpreadFade + 1)*fadeLevelDiff;
    currentOffset = getMinOffset(totalOffset - accumulatedOffset);
}

void Realignment::realign(std::shared_ptr<OrbishContext> context, OwnedArray<Track, CriticalSection>* tracks, bool hostHasPlayed){
    int df = int(context->quartersToSamples(context->info->ppqPosition)) % context->samplesPerQuarter;
    if (context->info->isPlaying
        && df < context->fadeTime) {
        if (!hostHasPlayed)hostHasPlayed = true;
        for(auto track:*tracks){
            if (track->IsPlaying() && !track->Reverse && track->getSnapMode() != kSnapNone){
                auto currentPosition = *track->CurrentPlayingIndex;
                double currentPosInQuarters =0;
                double currentHostPosInQuarters=0;
                double diffQuarters = 0;
                switch (track->getSnapMode()) {
                    case kSnapQuarter:
                        currentPosInQuarters = fmod(context->samplesToQuarters(currentPosition), 4 / context->timeSigBottom);
                        currentHostPosInQuarters = fmod(context->info->ppqPosition, 4 / context->timeSigBottom);
                        diffQuarters = currentHostPosInQuarters - currentPosInQuarters;
                        if (abs(diffQuarters) > (.5 * 4 / context->timeSigBottom)) {
                            if(currentPosInQuarters < currentHostPosInQuarters){
                                diffQuarters = currentHostPosInQuarters - (4 / context->timeSigBottom) - currentPosInQuarters ;
                            }else{
                                diffQuarters = (4 / context->timeSigBottom) - currentPosInQuarters + currentHostPosInQuarters;
                            }
                        }
                        break;
                    case kSnapMeasure:
                    case kSnapLoop:
                        currentPosInQuarters = fmod(context->samplesToQuarters(currentPosition), context->quartersPerBar());
                        currentHostPosInQuarters = fmod(context->info->ppqPosition, context->quartersPerBar());
                        diffQuarters = currentHostPosInQuarters - currentPosInQuarters;
                        if (abs(diffQuarters) > (context->quartersPerBar()*.5)) {
                            if(currentPosInQuarters < currentHostPosInQuarters){
                                diffQuarters = currentHostPosInQuarters - context->quartersPerBar() - currentPosInQuarters ;
                            }else{
                                diffQuarters = context->quartersPerBar() - currentPosInQuarters + currentHostPosInQuarters;
                            }
                        }
                        break;
                    default:
                        break;
                }
                int diff= context->quartersToSamples(diffQuarters);
                if (abs(diff) >= context->sampleRate / 400) {
                    if (!((diff < 0 && *track->CurrentPlayingIndex < abs(diff))
                          || (diff > 0 && *track->CurrentPlayingIndex > * track->LoopDuration - diff))) {
                        if (!track->realignment->isSyncInProgress()){
                            track->realignment->setTotalOffset(diff);
                        }
                    }
                }
#if DEBUG_LOG
                
                //            context->logMessage("#======================================");
                //            context->logMessage(String("#current pos in quarters:") + String(currentPosInQuarters));
                //            context->logMessage(String("#realign with host at:") + String(context->info->ppqPosition) + String("\tdiff:")+String(diff));
                //            context->logMessage(String("#current host pos in quarters:") + String(currentHostPosInQuarters));
                //
                //            context->logMessage(String("#diff in quarters:") + String(diffQuarters));
                //            context->logMessage(String("#hostSamples:") + String(context->info->timeInSamples));
                //            context->logMessage(String("#host position converted:") + String(int(context->quartersToSamples(context->info->ppqPosition))));
                //            context->logMessage(String("#host Samples tracked by p/i:") + String(trackHostSamples));
                //            context->logMessage(String("#host samples gap:") + String(trackHostSamples - context->info->timeInSamples));
                //            context->logMessage(String("#gap in quarters:") + String(context->samplesToQuarters(trackHostSamples - context->info->timeInSamples)));
                //
                //            context->logMessage(String("#plugin pos after host sync:") + String(*track->CurrentPlayingIndex));
                //            context->logMessage(String("#samples per beat:") + String(context->samplesPerBeat));
                //            context->logMessage(String("#samples per quarter:") + String(context->samplesPerQuarter));
                //            context->logMessage(String("#Loop duration:") + String(*track->LoopDuration));
                //            context->logMessage(String("#Loop duration in quarters:") + String(context->samplesToQuarters(*track->LoopDuration)));
                //            context->logMessage(String("#block in quarters:") + String(context->samplesToQuarters(context->maxBlockSize)));
                //            context->logMessage(String("#sample rate:") + String(context->sampleRate));
                //            context->logMessage(String("#samples per minute:") + String(context->samplesPerMinute));
                //            context->logMessage(String("#bpm:") + String(context->bpm));
                //            context->logMessage(String("#denominator:") + String(context->timeSigBottom));
#endif
            }
        }
    }
}
