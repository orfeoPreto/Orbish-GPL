//
//  LoopComponent.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 11/08/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "LoopComponent.h"

LoopComponent::LoopComponent(): progress(bidon){}
LoopComponent::LoopComponent(std::atomic<float>& p, int idx) :  progress(p){
    index = idx;
	setComponentID("Loop " + String(index));

//    setPercentageDisplay(false);
    thumbnail = std::make_unique<OpenGLAudioThumbnail>(p, true);
    thumbnail->setLookAndFeel(&getLookAndFeel());
//    thumbnail->setTopLevelComponent(this);
    thumbnail->setDisplayType(WaveDisplayType::kFlat);
    addAndMakeVisible(thumbnail.get());
    setOpaque(false);
    setAlpha(0.5);
    auto bounds = getLocalBounds().reduced(2*margin);
    thumbnail->setBounds(bounds.getX() + getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}

LoopComponent::~LoopComponent(){}

void LoopComponent::setActive(bool a){
    active =a;
    thumbnail->activate();
}
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
std::atomic<float>& LoopComponent::getProgress(){
    return progress;
}

void LoopComponent::mouseDrag() {
    
}

void LoopComponent::resized(){
    auto bounds = getLocalBounds().reduced(2*margin);
    thumbnail->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());

}

void LoopComponent::paint(Graphics& g){
    g.setColour(Colours::red);
    g.drawRect(getLocalBounds());
}
void LoopComponent::setMargin(int marg){
    margin = marg;
}

void LoopComponent::highlightBecomingActive() {
    if (thumbnail)
        thumbnail->prepareActivation();
 }

void LoopComponent::unHighlightBecomingActive() {
    if (thumbnail)
        thumbnail->unPrepareActivation();
}
