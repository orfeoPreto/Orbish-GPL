//
//  TrackGroup.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 16/08/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "TrackGroup.h"
#include <stdio.h>

    TrackGroup::TrackGroup(){}
	TrackGroup::TrackGroup(int index) {
		Index = index;
		Name = String::charToString(index + 65);
	}
    TrackGroup::~TrackGroup(){}
    
    void TrackGroup::AddTrack(Track* t){
        for (auto item : *this){
            if(t == item){
                return;
            }
        }
        this->std::vector<Track*>::push_back(t);
    }
    
    void TrackGroup::RemoveTrack(Track* t){
        for(auto i=0;i<this->size();i++){
            if(t == (*this)[i]){
                this->std::vector<Track*>::erase(this->std::vector<Track*>::begin() + i);
            }
        }
    }
    std::vector<Track>::size_type TrackGroup::size(){
       return this->std::vector<Track*>::size();
    }

    void TrackGroup::ForEach(std::function<void(Track*)> lambda){
        for (auto item : *this)
        {
            lambda.operator()(item);
        }
    }

bool TrackGroup::Contains(Track *t){
    bool found = false;
    ForEach([&found, t](Track* track){ found = (found || t == track); });
    return found;
}
