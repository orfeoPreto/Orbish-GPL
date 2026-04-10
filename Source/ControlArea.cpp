/*
  ==============================================================================

    ControlArea.cpp
    Created: 1 Aug 2020 3:22:34pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ControlArea.h"

//==============================================================================
ControlArea::ControlArea(){
    addAndMakeVisible(thumbnailAndGroupArea);
    addAndMakeVisible(groupControlArea);
    addAndMakeVisible(buttonControlArea);
//    setOpaque(true);
   // setAlpha(0.99);
}

ControlArea::~ControlArea(){
}

void ControlArea::paint (juce::Graphics&){
}

void ControlArea::resized(){
    auto bounds = getLocalBounds();

    // Band 2: Waveform - full width, ~1/3 of available height
    auto waveformHeight = juce::jmax(100, (int)(bounds.getHeight() / 3));
    thumbnailAndGroupArea.setBounds(bounds.removeFromTop(waveformHeight));

    // GroupControlArea hidden (group controls move to nav panel)
    groupControlArea.setBounds(0, 0, 0, 0);

    // Band 3: Control rail - remaining space
    buttonControlArea.setBounds(bounds);
}

//void ControlArea::setLookAndFeel (LookAndFeel* const newLookAndFeel){
////    Component::setLookAndFeel(newLookAndFeel);
//    thumbnailAndGroupArea.setLookAndFeel(newLookAndFeel);
//    groupControlArea.setLookAndFeel(newLookAndFeel);
//    buttonControlArea.setLookAndFeel(newLookAndFeel);
//}
