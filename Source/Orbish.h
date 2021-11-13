//
//  Orbish.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 23/03/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef __Orbish_H_INCLUDED
#define __Orbish_H_INCLUDED
#define LAYERS_VISIBLE  5
#define BUFFER_READ_SIZE 1000
#define JUCE_MODAL_LOOPS_PERMITTED true
//#define DEBUG_LOG  1

#include <vector>
#include <string>
#include <thread>
#include "JuceHeader.h"
//#include <rubberband/RubberBandStretcher.h>

enum InputParamsIndexes
{
	kAll=-2,
	kNone=-1,
    kRecordParam=0,
    kPlayParam,
    kResetParam,
    kRecTriggerMode,
    kRecMode,
    kSnapMode,
    kReverse,
    kMonitorParam,
    kMixParam,
    kTrackParam,
	kMute,
	kStop,
	kDelay,
	kInputLevel,
	kOutputLevel,
	kPreviousParam,
	kNextParam,
	kBounceParam,

	kMuteAll,
	kStopAll,
	kPauseAll,
	kShrinkParam
};

enum RecordMode
{
    kRecLoopOver=0, ///< standard looper: records over existing material and keep original loop length
    kRecFixedLoopOver, ///< standard looper - start with fixed size
    kRecExtend,     ///< records over existing material and extends original loop length when reaching end of loop
    kRecAppend,     ///< append to existing loop (without recording original loop content after loop duration has been reached)
    kRecOverWrite,  ///< overwrite loop content with new material, and extend original loop until activeTrack.Recording ends
    kRecPunch,     ///< overwrite loop content with new material, but keeps original loop length
    kFirstRec = kRecLoopOver,
    kLastRec = kRecPunch
};

enum SnapMode
{
    kSnapNone=0,
    kSnapMeasure,
    kSnapQuarter,
    kSnapLoop,
    kSnapHostLoop,
    kFirstSnap=kSnapNone,
    kLastSnap=kSnapHostLoop
};

//==============================================================================
/**
 */
struct Events{
    int startRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be started
    int stopRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be stopped
    int startPlayingSample = -1; // sample number in buffer when playback should be started
    int stopPlayingSample = -1; // sample number in buffer when playback should be stopped
    int startReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be started
    int stopReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be stopped
    int toggleMuteSample = -1; //sample number in buffer when Mute state should be toggled
    int toggleSoloSample = -1; //sample number in buffer when Solo state should be toggled
    int startLoopChangeSample = -1;
    bool toggleSolo = false;
};

// determines if 2 doubles are equal
//taking into account that there might be a difference that is smaller than machine epsilon
// (the smallest difference representable by a double)
inline bool doublesEqual(double dFirstVal, double dSecondVal)
{
    return std::abs(dFirstVal - dSecondVal) < std::numeric_limits<double>::epsilon();
}

#endif
