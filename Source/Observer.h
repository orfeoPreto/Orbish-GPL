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
    
    virtual void updateLoopVisualiser (const AudioBuffer<float>& buffer, int numSamples) {};
    virtual void askToUpdatePlayHead(int position, bool reverse) {};
    virtual void askToHandleMidiMessages(const MidiBuffer& midiMessages) {};
    
    virtual void askToCreateTrack() {};
    virtual void askToChangeTrack(int trackNumber) {};
    virtual void askToRemoveTrack(int trackNumber) {};
    
    virtual void askToCreateLoop() {};
    virtual void askToChangeLoop(int trackNumber, int loopNumber) {};
    virtual void askToRemoveLoop() {};

    virtual void askToUpdatePlayState(int trackNumber) {};

};
#endif /* Observer_hpp */
