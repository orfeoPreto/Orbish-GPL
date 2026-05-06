// PluginProcessorCommands.cpp
// Split compilation unit for OrbishAudioProcessor - non-RT command handlers.
// These run on the message thread in response to parameter changes.

#include "PluginProcessor.h"
#include "PluginEditor.h"

void OrbishAudioProcessor::parameterChanged(const String& parameterID, float newValue) {
    try {
    if (parameterID == "globalMix") {
        context->mix = Decibels::decibelsToGain(newValue);
    }
    if (parameterID == "clickLevel") {
        auto p = parameters.getParameter("clickLevel");
        context->clickLevel =  p->convertTo0to1(newValue);
    }
    if (parameterID == "latency") {
        auto delay = newValue * context->sampleRate * 0.001f;
        context->delayCompensation = int(delay);
    }
    if (parameterID == "muteAll") {
        processMuteAllChange(bool(newValue));
    }
    if (parameterID == "trackSelect") {
        if (newValue >= 0) {
            processTrackChange(uint(newValue));
        }
    }
    if (parameterID == "nextTrack") {
        askTrackChange(activeTrack->Index + 1);
    }
    if (parameterID == "previousTrack") {
        askTrackChange(activeTrack->Index - 1);
    }
    if (parameterID == "newTrack") {
        processNewTrack();
    }
    if (parameterID == "click") {
        context->clickEnabled = (newValue >= 0.5f);
    }
    if (parameterID == "removeTrack") {
        processRemoveTrack(tracks.size() - 1);
    }
    if (parameterID == "stopAll") {
        processStopAllChange();
    }
    if (parameterID == "startAll") {
        processStartAllChange();
    }
    if (parameterID == "pauseAll") {
        processPauseAllChange(bool(newValue));
    }
    if (parameterID == "resetAll") {
        processResetAllChange();
    }
    if (parameterID == "selectGroup") {
        processGroupSelect(int(newValue));
    }
    if (parameterID == "addToGroup") {
        processAddToGroup(activeTrack->Index);
    }
    if (parameterID == "removeFromGroup") {
        processRemoveFromGroup(activeTrack->Index);
    }
    } catch (...) {
        DBG("Exception in OrbishAudioProcessor::parameterChanged for: " + parameterID);
    }
}

void OrbishAudioProcessor::startReset()
{
    // start playback of all armed tracks
    for (auto track : tracks) {
        if (*track->LoopDuration > 0) {
            track->StartResetBefore();
        }
    }
}

void OrbishAudioProcessor::startPlayback()
{
    // start playback of all armed tracks
    for (auto track : tracks) {
        if (!track->Playing) {
            track->StartPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::stopPlayback()
{
    // stop playback of all tracks currently playing
    for (auto track : tracks) {
        if (track->Playing)
            track->StopPlaybackBefore();
    }
    if (activeTrack->Recording) {
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::resumePlayback()
{
    // resume playback of all armed tracks
    for (auto track : tracks) {
        track->StartPlaybackBefore();
    }
}

void OrbishAudioProcessor::pausePlayback()
{
    // pause playback of all tracks currently playing
    for (auto track : tracks) {
        if (track->Playing)
            track->PausePlaybackBefore();
    }
}

void OrbishAudioProcessor::doMute() {
    for (auto track : tracks) {
        if (track->isMuteArmed()) {
            track->StartMuteBefore();
        }
        else {
            track->StopMuteBefore();
        }
    }
}

void OrbishAudioProcessor::processNewTrack() {
    addTrack(false);
    if (guiAlive) {
            (context->observer->*(context->observer->newTrack))();
    }
}


void OrbishAudioProcessor::processRemoveTrack(int trackNumber) {
    if (trackNumber > 0 && trackNumber < tracks.size()) {
        if (tracks[trackNumber]->Recording) {
            tracks[trackNumber]->StopRecordingBefore();
        }
        if (tracks[trackNumber]->Playing) {
            tracks[trackNumber]->StopPlaybackBefore();
        }
        trackChange.trackToRemove = trackNumber;
        if (guiAlive) {
            (context->observer->*(context->observer->trackRemoval)) (trackNumber);
        }
    }
}

void OrbishAudioProcessor::processStopAllChange() {
    globalAction = kGlobalStop;
    // stop playback of all armed tracks
    for (auto track : tracks) {
        track->setPlayArmed(false);
        if (track->getSnapMode() == kSnapNone)
        {
            track->StopPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::processResetAllChange() {
    startReset();
    if (activeTrack->Recording) {
        activeTrack->StopRecordingBefore();
    }
}

void OrbishAudioProcessor::processStartAllChange() {
    globalAction = kGlobalPlay;
    // start playback of all armed tracks
    for (auto track : tracks) {
        track->setPlayArmed(true);
        if (track->getSnapMode() == kSnapNone)
        {
            track->StartPlaybackBefore();
        }
    }
}

void OrbishAudioProcessor::processPauseAllChange(bool pause) {
    if (pause) {
        globalAction = kGlobalPause;
    }
    else {
        globalAction = kGlobalResume;
    }
    for (auto track : tracks) {
        if (!pause) {
            if (track->WasPlaying) {
                track->setPlayArmed(true);
                if (track->getSnapMode() == kSnapNone)
                {
                    track->StartPlaybackBefore();
                }
            }
        }
        else {
            if (track->Playing) {
                track->setPlayArmed(false);
                if (track->getSnapMode() == kSnapNone)
                {
                    track->PausePlaybackBefore();
                }
            }
        }
    }
}

void OrbishAudioProcessor::processMuteAllChange(bool mute) {
    globalAction = kGlobalMute;
    for (auto track : tracks) {
        track->setMuteArmed(mute);
        if (track->isActive()) {
            track->setRecordingArmed(track->isRecordingArmed() && !track->isMuteArmed());
            if (track->getSnapMode() == kSnapNone)
            {
                track->Muted = track->isMuteArmed();
                if (track->Recording && track->Muted)
                    track->StopRecordingBefore();
            }
        }
    }
}

void OrbishAudioProcessor::processTrackChange(uint newTrackIndex) {
    if (activeTrack->Index != newTrackIndex && newTrackIndex < uint(tracks.size())) {
        // disable recording on track we're about to leave
        if (activeTrack->Recording) {
            activeTrack->StopRecordingBefore();
        }
        trackChange.nextTrack = newTrackIndex;
    }
}

void OrbishAudioProcessor::processGroupSelect(int groupIdx) {
    if (groupIdx >= 0 && groupIdx < (int)groups.size())
        SelectedGroup = groups[groupIdx];
}

void OrbishAudioProcessor::processAddToGroup(int track) {
    CurrentGroup->RemoveTrack(tracks[track]);
    SelectedGroup->AddTrack(tracks[track]);
    CurrentGroup = SelectedGroup;
}

void OrbishAudioProcessor::processRemoveFromGroup(int track) {
    CurrentGroup->RemoveTrack(tracks[track]);
}

void OrbishAudioProcessor::processTempoChange(double bpm) {
    // RubberBand time-stretching removed (GPL release).
    // Tempo change is recorded but no longer drives a stretcher.
    (void)bpm;
}

void OrbishAudioProcessor::initGroups() {
    while (groups.size() > 0) {
        groups.pop_back();
    }
    for (auto i = 0; i < 10; ++i) {
        groups.push_back(new TrackGroup(i));
    }
    SelectedGroup = CurrentGroup = groups.front();
}
