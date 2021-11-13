//
//  EventHandler.h
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 09/11/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef EventHandler_h
#define EventHandler_h
#include "Orbish.h"
enum GlobalAction{
    kGlobalNone,
    kGlobalStop,
    kGlobalPlay,
    kGlobalPause,
    kGlobalResume,
    kGlobalMute,
    kGlobalReset
};

class TrackEventHandler{
public:
    TrackEventHandler(){}
    virtual ~TrackEventHandler(){}
    
    virtual void handleReverseEvent(int, int)=0;
    virtual void handleRecordingEvent(int, int)=0;
    virtual void handlePlaybackEvent(int, int)=0;
    virtual void handleMuteEvent(int)=0;
    virtual bool handleSoloEvent(int)=0;
    virtual void handleLoopChangeEvent(int)=0;
    virtual void captureTrigger(int& startRecordingSample)=0;
    void handleEvents(Events& e, Track* trackToSync){
        captureTrigger(e.startRecordingSample);
        
        // actions will be performed in sync based on the snap mode
        // look for the sample corresponding to the snap (bar or beat) and perform action
        // if no sync the actions are performed immediately
        
        //print2("timeSigBottom:", data.transport->timeSigBottom);
        // start position is either buffer start, or startRecordingSample if we are detecting automatically
        int nextSyncPoint=-1;
        nextSyncPoint = trackToSync->getNextSynchronizationPoint(trackToSync->getSnapMode());
        if(nextSyncPoint < 0){
            nextSyncPoint = primarySynchronizer->getNextSynchronizationPoint(trackToSync->getSnapMode());
        }
#if DEBUG_LOG
        
        //context->logMessage("currentPos: " + String(currentPos));
        //context->logMessage("expectedSamplePositi on: " + String(int64(expectedSamplePosition)));
        //context->logMessage("expectedPos: " + String(expectedPos));
        
        //context->logMessage("nextSample" + String(nextSample));
        //context->logMessage("timeInSamples:" + String(context->info->timeInSamples));
        //context->logMessage("timeInSecs: " + String(context->info->timeInSeconds));
        //context->logMessage("currentRecordingIndex:" + String(activeTrack->CurrentRecordingIndex));
        //context->logMessage("currentPlayingIndex:" + String(*activeTrack->CurrentPlayingIndex));
        
        //context->logMessage("active track loop duration:" + String(*activeTrack->LoopDuration));
#endif
        // manage activeTrack->Recording
        //    int diff = context->maxBlockSize - context->samplesPerBlock;
        if (!trackToSync->Recording && trackToSync->isRecordingArmed())
            {
            if((trackToSync->getSnapMode() == kSnapNone
                && trackToSync->isAutoTrigger())){
                if (!trackToSync->Triggered) {
                    e.startRecordingSample = -1;
                }
            }else{
                e.startRecordingSample = nextSyncPoint;
            }
            }
        else if (trackToSync->Recording && !trackToSync->isRecordingArmed())
            {
            e.stopRecordingSample = nextSyncPoint;
            }
        // manage activeTrack->Playing
        if ((!trackToSync->Playing && trackToSync->isPlayArmed()) || globalAction == kGlobalPlay || globalAction == kGlobalResume)
            {
            e.startPlayingSample = nextSyncPoint;
            }
        else if ((trackToSync->Playing && !trackToSync->isPlayArmed()) || globalAction == kGlobalStop || globalAction == kGlobalPause)
            {
            e.stopPlayingSample = nextSyncPoint;
            }
        
        // manage reversing
        if (!trackToSync->Reverse && trackToSync->isReverseArmed())
            {
            e.startReverseSample = nextSyncPoint;
            }
        
        else if (trackToSync->Playing && trackToSync->Reverse && !trackToSync->isReverseArmed()) {
            e.stopReverseSample = nextSyncPoint;
        }
        if (trackToSync->loopChangeArmed)
            {
            e.startLoopChangeSample = nextSyncPoint;
            }
        // manage Mute
        if ((trackToSync->isMuteArmed() != trackToSync->Muted) || globalAction == kGlobalMute) {
            e.toggleMuteSample = nextSyncPoint;
        }
        // manage Solo
        if (trackToSync->isSoloArmed() != trackToSync->Soloed) {
            e.toggleSoloSample = nextSyncPoint;
        }
        handlePlaybackEvent(e.startPlayingSample, e.stopPlayingSample);
        handleRecordingEvent(e.startRecordingSample, e.stopRecordingSample);
        // manage activeTrack->Reverse state
        handleReverseEvent(e.startReverseSample, e.stopReverseSample);
        // manage mute state
        handleMuteEvent(e.toggleMuteSample);
        
        handleLoopChangeEvent(e.startLoopChangeSample);
        // manage solo state
        e.toggleSolo = handleSoloEvent(e.toggleSoloSample);
    }
    
    GlobalAction globalAction = kGlobalNone;
    std::unique_ptr<Synchronizer> primarySynchronizer;
    std::unique_ptr<Synchronizer> secondarySynchronizer;
};

#endif /* EventHandler_h */
