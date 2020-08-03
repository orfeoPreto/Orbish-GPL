/*
  ==============================================================================

    ControlArea.cpp
    Created: 1 Aug 2020 3:22:34pm
    Author:  lenna

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ControlArea.h"

//==============================================================================
ControlArea::ControlArea(){
    addAndMakeVisible(thumbnailAndGroupArea);
    addAndMakeVisible(buttonControlArea);
}

ControlArea::~ControlArea(){
}

void ControlArea::paint (juce::Graphics& g){
}

void ControlArea::resized(){
    auto bounds = getLocalBounds();
    thumbnailAndGroupArea.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    buttonControlArea.setBounds(bounds);
}
