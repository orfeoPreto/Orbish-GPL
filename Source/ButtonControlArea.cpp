/*
  ==============================================================================

    ButtonControlArea.cpp
    Created: 3 Aug 2020 2:12:34pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ButtonControlArea.h"

//==============================================================================
ButtonControlArea::ButtonControlArea(){
    addAndMakeVisible(transportControlArea);
    addAndMakeVisible(modeAndNavigationControlArea);
    addAndMakeVisible(globalControlArea);
    addAndMakeVisible(outputControlArea);
    addAndMakeVisible(inputControlArea);
}

ButtonControlArea::~ButtonControlArea(){
}

void ButtonControlArea::paint (juce::Graphics& g){
}

void ButtonControlArea::resized(){
    auto bounds = getLocalBounds();
    auto controlWidth = bounds.getWidth() / 4;
    transportControlArea.setBounds(bounds.removeFromLeft(controlWidth));

    auto leftoverWidth = bounds.getWidth() / 4;
    modeAndNavigationControlArea.setBounds(bounds.removeFromLeft(leftoverWidth));
    globalControlArea.setBounds(bounds.removeFromLeft(leftoverWidth));
    outputControlArea.setBounds(bounds.removeFromLeft(leftoverWidth));
    inputControlArea.setBounds(bounds);
}
