/*
  ==============================================================================

    InfoAndControlArea.cpp
    Created: 1 Aug 2020 12:47:40pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InfoAndControlArea.h"
#include "PluginEditor.h"

//==============================================================================
InfoAndControlArea::InfoAndControlArea(){
    addAndMakeVisible(infoArea);
    addAndMakeVisible(controlArea);
}

InfoAndControlArea::~InfoAndControlArea(){
}

void InfoAndControlArea::paint (juce::Graphics&){
}

void InfoAndControlArea::resized(){
    auto bounds = getLocalBounds();
    infoArea.setBounds(bounds.removeFromLeft(150));
    controlArea.setBounds(bounds);
}
