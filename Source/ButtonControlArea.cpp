/*
  ==============================================================================

    ButtonControlArea.cpp
    Created: 3 Aug 2020 2:12:34pm
    Author:  Aoriseth

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
    auto controlWidth = bounds.getWidth() / 5;
    transportControlArea.setBounds(bounds.removeFromLeft(controlWidth));
    modeAndNavigationControlArea.setBounds(bounds.removeFromLeft(controlWidth));
    globalControlArea.setBounds(bounds.removeFromLeft(controlWidth));
    outputControlArea.setBounds(bounds.removeFromLeft(controlWidth));
    inputControlArea.setBounds(bounds);
}
