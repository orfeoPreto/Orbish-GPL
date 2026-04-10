// PluginProcessorEvents.cpp
// Split compilation unit for OrbishAudioProcessor - RT event handlers.
// These are called from processBlock via the TrackEventHandler system.

#include "PluginProcessor.h"

void OrbishAudioProcessor::captureTrigger(int& startRecordingSyncPoint){
    // Auto Trigger mode: check if there is any sound before starting new activeTrack->Recording
    if (!activeTrack->Recording
        && activeTrack->isRecordingArmed()
        && activeTrack->isAutoTrigger()
        && !activeTrack->Triggered)
    {

         const float* const* samples = context->buffer->getArrayOfReadPointers();
        for (uint32 channel = 0; channel < context->audioInputsCount && startRecordingSyncPoint == -1; channel++)

            for (int i = 0; i < context->maxBlockSize && startRecordingSyncPoint == -1; i++)
            {
                if (abs(samples[channel][i]) > context->triggerThreshold)
                {
                    startRecordingSyncPoint = i;
                    activeTrack->Triggered = true;
                }
            }
    }
}


void OrbishAudioProcessor::handleReverseEvent(int startReverseSyncPoint, int stopReverseSyncPoint){
    // manage activeTrack->Reverse state

    if (primarySynchronizer->isSyncEventImminent(startReverseSyncPoint)) {
        if (nullptr != getTrackGroup(activeTrack)) {
            for(auto groupedTrack:*CurrentGroup){
                groupedTrack->StartReverse();
            }
        }
        else {
            activeTrack->StartReverse();
        }
    }
    else if (primarySynchronizer->isSyncEventImminent(stopReverseSyncPoint)) {
        if (nullptr != getTrackGroup(activeTrack)) {
            for(auto groupedTrack:*CurrentGroup){
                groupedTrack->StopReverse();
            }
        }
        else {
            activeTrack->StopReverse();
        }

    }
}

void OrbishAudioProcessor::handleRecordingEvent(int startRecordingSyncPoint, int stopRecordingSyncPoint){
    if (primarySynchronizer->isSyncEventImminent(startRecordingSyncPoint)) {
        activeTrack->StartRecordingBefore();
    }
    else if (primarySynchronizer->isSyncEventImminent(stopRecordingSyncPoint)) {
        // switch recording off but still allow for the last samples to be written + fade
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::handlePlaybackEvent(int startPlayingSyncPoint, int stopPlayingSyncPoint){
    if (primarySynchronizer->isSyncEventImminent(startPlayingSyncPoint))
    {
        if (globalAction == kGlobalPlay) {
            startPlayback();
        }
        else if (globalAction == kGlobalResume) {
            resumePlayback();
        }
        else {
            if (nullptr != getTrackGroup(activeTrack)) {
                for(auto groupedTrack:*CurrentGroup){
                    groupedTrack->StartPlaybackBefore();
                }
            }
            else {
                activeTrack->StartPlaybackBefore();
            }
        }
        globalAction = kGlobalNone;
    }
    else if (primarySynchronizer->isSyncEventImminent(stopPlayingSyncPoint))
    {
        if (globalAction == kGlobalStop) {
            stopPlayback();
        }
        else if (globalAction == kGlobalPause) {
            pausePlayback();
        }
        else {
            if (activeTrack->isStopArmed())
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StopPlaybackBefore();
                    }
                }
                else {
                    activeTrack->StopPlaybackBefore();
                }
                else
                    if (nullptr != getTrackGroup(activeTrack)) {
                        for(auto groupedTrack:*CurrentGroup){
                            groupedTrack->PausePlaybackBefore();
                        }
                    }
                    else {
                        activeTrack->PausePlaybackBefore();
                    }
        }
        globalAction = kGlobalNone;
    }
}


void OrbishAudioProcessor::handleMuteEvent(int toggleMuteSyncPoint){
    if (primarySynchronizer->isSyncEventImminent(toggleMuteSyncPoint)) {
        if (globalAction == kGlobalMute) {
            doMute();
        }
        else {
            if (activeTrack->isMuteArmed()) {
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StartMuteBefore();
                    }
                }
                else {
                    activeTrack->StartMuteBefore();
                }
            }
            else {
                if (nullptr != getTrackGroup(activeTrack)) {
                    for(auto groupedTrack:*CurrentGroup){
                        groupedTrack->StopMuteBefore();
                    }
                }
                else {
                    activeTrack->StopMuteBefore();
                }
            }
        }
        globalAction = kGlobalNone;
    }
}

bool OrbishAudioProcessor::handleSoloEvent(int toggleSoloSyncPoint){
    auto toggleSolo = primarySynchronizer->isSyncEventImminent(toggleSoloSyncPoint);
    if (toggleSolo) {
        if (activeTrack->isSoloArmed()) {
            mixState.aTrackIsSoloed = true;
            if (nullptr != getTrackGroup(activeTrack)) {
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if(!CurrentGroup->Contains(*track)){
                        if((*track)->Soloed){
                            (*track)->StopSoloBefore();
                        }
                        (*track)->FirstMuteBuffer = true;
                    }
                }
                CurrentGroup->ForEach([](Track* t){ t->StartSoloBefore();} );
            }
            else {
                activeTrack->StartSoloBefore();
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if((*track)->Soloed){
                        (*track)->StopSoloBefore();
                    }
                    (*track)->FirstMuteBuffer = true;
                }
            }
        }
        else {
            mixState.aTrackIsSoloed = false;
            if (nullptr != getTrackGroup(activeTrack)) {
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    if(!CurrentGroup->Contains(*track)){
                        (*track)->LastMuteBuffer = true;
                    }
                }
                CurrentGroup->ForEach([](Track* t){ t->StopSoloBefore();} );
            }
            else {
                activeTrack->StopSoloBefore();
                for (auto track = tracks.begin(); track != tracks.end() && (*track)->Index < uint(tracks.size()); ++track) {
                    (*track)->LastMuteBuffer = true;
                }
            }
        }
    }
    return toggleSolo;
}

void OrbishAudioProcessor::handleLoopChangeEvent(int startLoopChangeSyncPoint){
        if (primarySynchronizer->isSyncEventImminent(startLoopChangeSyncPoint)) {
        if (nullptr != getTrackGroup(activeTrack)) {
            int diff = activeTrack->nextLoop - activeTrack->ActiveLoop->Index;
            for(auto groupedTrack:*CurrentGroup){
                auto newIdx = groupedTrack->ActiveLoop->Index + diff % groupedTrack->loops.size();
                    groupedTrack->ChangeLoopBefore(newIdx);
            }
        }
        else {
            activeTrack->ChangeLoopBefore(activeTrack->nextLoop);
        }
    }
}
