//
//  TrackLoader.hpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 09/11/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef TrackLoader_h
#define TrackLoader_h
#include "Track.h"
#include <stdio.h>
#include "PluginProcessor.h"


class TrackLoader{
    
public:
    TrackLoader(OrbishAudioProcessor& processor);
    ~TrackLoader();
    bool loadFromValueTree(ValueTree* tree);
    bool loadTrackFromValueTree(ValueTree*, Track*);
    bool loadLoopFromValueTree(ValueTree*, Loop*, Track*);
    OwnedArray<Track, CriticalSection>* tracks;
    std::shared_ptr<OrbishContext> context;
    OrbishAudioProcessor& processor;

};
#endif /* TrackLoader_hpp */
