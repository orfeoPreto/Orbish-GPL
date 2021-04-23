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
#include "DataExchange.h"


class Observer{
public:
     virtual ~Observer() {}
     Observer() {}

    typedef void (Observer::*UpdateBuffer) (std::shared_ptr<BufferForVisualisation>);
    UpdateBuffer updateLoopBuffer = 0;
    typedef void (Observer::*UpdatePosition) (int, bool);
    UpdatePosition updatePlayPosition = 0;
    typedef void (Observer::*HandleMidi) (const MidiBuffer&);
    HandleMidi handleMidi = 0;
    typedef void (Observer::*NewTrack) ();
    NewTrack newTrack = 0;
    typedef void (Observer::*TrackChange) (int);
    TrackChange trackChange = 0;
    typedef void (Observer::*TrackRemoval) (int);
    TrackRemoval trackRemoval = 0;
    typedef void (Observer::*NewLoop) ();
    NewLoop newLoop = 0;
    typedef void (Observer::*LoopChange) (int, int);
    LoopChange loopChange = 0;
    typedef void (Observer::*LoopRemoval) ();
    LoopRemoval loopRemoval = 0;
    typedef void (Observer::*LayerChange) (int, int);
    LoopChange layerChange = 0;
    typedef void (Observer::* PlayChanged)(int);
    PlayChanged playChanged = 0;
    typedef void (Observer::* HostPositionChanged)(int);
    HostPositionChanged hostPositionChanged = 0;
	typedef void (Observer::* LayersRefreshed)();
	LayersRefreshed layersRefreshed = 0;


    virtual void updateLoopVisualiser (std::shared_ptr<BufferForVisualisation>) {};
    virtual void askToUpdatePlayHead(int , bool ) {};
    virtual void askToHandleMidiMessages(const MidiBuffer& ) {};
    virtual void askToCreateTrack() {};
    virtual void askToChangeTrack(int ) {};
    virtual void askToRemoveTrack(int ) {};
    virtual void askToCreateLoop() {};
    virtual void askToChangeLoop(int , int ) {};
    virtual void askToRemoveLoop() {};
    virtual void askToChangeLayer(int, int ) {};
    virtual void askToUpdatePlayState(int ) {};
    virtual void askToUpdateHostPosition(int ) {};
	virtual void askToRefreshThumbnail() {};

};
#endif /* Observer_hpp */
