//
//  LoopComponent.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 11/08/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "LoopComponent.h"

LoopComponent::LoopComponent():ProgressBar(bidon), progress(bidon){}
LoopComponent::LoopComponent(double& p, int idx) : ProgressBar(p), progress(p){
    index = idx;
    setPercentageDisplay(false);
}

LoopComponent::~LoopComponent(){}

void LoopComponent::setActive(bool a){active =a;}
bool LoopComponent::isActive(){
    return active;
}
bool LoopComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    if (dragSourceDetails.description == "loop") {
        return true;
    }
    else {
        return false;
    }
}
void LoopComponent::itemDragEnter(const SourceDetails& dragSourceDetails) {
    this->setAlpha(0.5);
    repaint();
}
void LoopComponent::itemDragMove(const SourceDetails& dragSourceDetails) {}
void LoopComponent::itemDragExit(const SourceDetails& dragSourceDetails) {
    this->setAlpha(1);
    repaint();
}
void LoopComponent::itemDropped(const SourceDetails& dragSourceDetails) {
    this->setAlpha(1);
    repaint();
}
bool LoopComponent::shouldDrawDragImageWhenOver() {
    return true;
}
int LoopComponent::getIndex(){
    return index;
}
double& LoopComponent::getProgress(){
    return progress;
}

void LoopComponent::mouseDrag() {
    
}

