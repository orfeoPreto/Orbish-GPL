/*
  ==============================================================================

    ThumbnailAndGroupArea.cpp
    Created: 3 Aug 2020 2:13:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ThumbnailAndGroupArea.h"

//==============================================================================
ThumbnailAndGroupArea::ThumbnailAndGroupArea(){
    addAndMakeVisible(thumbnailArea);
    addAndMakeVisible(groupControlArea);
}

ThumbnailAndGroupArea::~ThumbnailAndGroupArea(){
}

void ThumbnailAndGroupArea::paint (juce::Graphics& g){

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void ThumbnailAndGroupArea::resized(){
    auto bounds = getLocalBounds().reduced(10);

    groupControlArea.setBounds(bounds.removeFromRight(bounds.getWidth() / 10));
    thumbnailArea.setBounds(bounds);
}
