/*
  ==============================================================================

    ModeAndNavigationControlArea.cpp
    Created: 3 Aug 2020 2:16:27pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModeAndNavigationControlArea.h"

//==============================================================================
ModeAndNavigationControlArea::ModeAndNavigationControlArea(){
    addAndMakeVisible(modeControlArea);
    addAndMakeVisible(navigationControlArea);
}

ModeAndNavigationControlArea::~ModeAndNavigationControlArea(){
}

void ModeAndNavigationControlArea::paint (juce::Graphics& g){
}

void ModeAndNavigationControlArea::resized(){
    auto bounds = getLocalBounds().reduced(10);

    modeControlArea.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    navigationControlArea.setBounds(bounds);
}
