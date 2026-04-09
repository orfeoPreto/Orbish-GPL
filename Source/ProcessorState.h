#pragma once

#include "JuceHeader.h"

// Plain data structs grouping related processor state.
// Zero overhead: identical memory layout to flat member variables.

struct ClickState {
    bool inProcess = false;
    int sourcePosition = 0;
    AudioSampleBuffer* activeSource = nullptr;
    std::atomic<float> downbeat{0.0f};  // 1.0 on bar start click, 0.0 otherwise
};

struct TrackChangeState {
    int nextTrack = -1;
    int trackToRemove = -1;
    bool changing = false;
};

struct SyncState {
    bool hostHasPlayed = false;
    int trackHostSamples = 0;
    int64 standaloneSamplePosition = 0;
};

struct MixState {
    bool aTrackIsSoloed = false;
    bool queuesEmpty = false;
    double previousMixLevel = -1;
};
