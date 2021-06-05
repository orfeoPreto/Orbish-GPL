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
    kRecPunch     ///< overwrite loop content with new material, but keeps original loop length
};

enum SnapMode
{
    kSnapNone=0,
    kSnapMeasure,
    kSnapQuarter
};

inline bool doublesEqual(double dFirstVal, double dSecondVal)
{
    return std::abs(dFirstVal - dSecondVal) < std::numeric_limits<double>::epsilon();
}
#endif
