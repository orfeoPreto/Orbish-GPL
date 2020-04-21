//
//  TrackGroup.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 16/08/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef TrackGroup_hpp
#define TrackGroup_hpp

#include <stdio.h>
#include "Track.h"




class TrackGroup:private std::vector<Track*> {
public:
    TrackGroup();
	TrackGroup(int);
    ~TrackGroup();
    
    int Index = 0;
	String Name;
    void AddTrack(Track* t);
    bool Contains(Track* t);
    void ForEach(std::function<void(Track*)> lambda);

    void RemoveTrack(Track* t);
    size_type size();

    using vector::operator[];
    using vector::begin;
    using vector::end;
};
#endif /* TrackGroup_hpp */
