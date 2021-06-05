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
    auto r = bounds.removeFromTop(bounds.getHeight() / 2);
    thumbnailAndGroupArea.setBounds( r.removeFromLeft(juce::jmax((r.getWidth() * (3/4)), r.getWidth() - 160)));
    groupControlArea.setBounds(r);

    buttonControlArea.setBounds(bounds);
}

//void ControlArea::setLookAndFeel (LookAndFeel* const newLookAndFeel){
////    Component::setLookAndFeel(newLookAndFeel);
//    thumbnailAndGroupArea.setLookAndFeel(newLookAndFeel);
//    groupControlArea.setLookAndFeel(newLookAndFeel);
//    buttonControlArea.setLookAndFeel(newLookAndFeel);
//}
