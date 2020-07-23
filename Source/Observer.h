//
//  Observer.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 28/04/2019.
//  Copyright © 2019 EXU. All rights reserved.
//
#pragma once

#ifndef Observer_h
#define Observer_h

#include <stdio.h>
#include "JuceHeader.h"


class Observer{
public:
     virtual ~Observer() {}
     Observer() {}

    typedef void (Observer::*UpdateBuffer) (const AudioBuffer<float>&, int);
    UpdateBuffer updateInputBuffer;
    UpdateBuffer updateLoopBuffer;
    typedef void (Observer::*UpdatePosition) (int, bool);
    UpdatePosition updatePlayPosition;
    typedef void (Observer::*HandleMidi) (const MidiBuffer&);
    HandleMidi handleMidi;
    typedef void (Observer::*NewTrack) ();
    NewTrack newTrack;
    typedef void (Observer::*TrackChange) (int);
    TrackChange trackChange;
    typedef void (Observer::*TrackRemoval) (int);
    TrackRemoval trackRemoval;
    
    typedef void (Observer::*NewLoop) ();
    NewLoop newLoop;
    typedef void (Observer::*LoopChange) (int, int);
    LoopChange loopChange;
    typedef void (Observer::*LoopRemoval) ();
    LoopRemoval loopRemoval;

    typedef void (Observer::* PlayChanged)(int);
    PlayChanged playChanged;
    
    virtual void updateInputVisualiser (const AudioBuffer<float>& buffer, int numSamples) {};
    virtual void updateLoopVisualiser (const AudioBuffer<float>& buffer, int numSamples) {};
    virtual void updatePlayHead(int position, bool reverse) {};
    virtual void handleMidiMessages(const MidiBuffer& midiMessages) {};
    
    virtual void askToCreateTrack() {};
    virtual void updateNextTrackNumber(int trackNumber) {};
    virtual void removeTrack(int trackNumber) {};
    
    virtual void askToCreateLoop() {};
    virtual void updateNextLoopNumber(int trackNumber, int loopNumber) {};
    virtual void removeLoop() {};

    virtual void updatePlaying(int trackNumber) {};

};
#endif /* Observer_hpp */
