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
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible(thumbnailAndGroupArea);
    addAndMakeVisible(buttonControlArea);

}

ControlArea::~ControlArea(){
}

void ControlArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::green);
    g.drawRect (thumbnailAndGroupArea.getBoundsInParent(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::yellow);
    g.drawRect(buttonControlArea.getBoundsInParent(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("thumbNailAndGroupArea", thumbnailAndGroupArea.getBoundsInParent(), juce::Justification::centred, true);   // draw some placeholder text
    g.drawText("buttonControlArea", buttonControlArea.getBoundsInParent(), juce::Justification::centred, true);   // draw some placeholder text
}

void ControlArea::resized(){
    auto bounds = getLocalBounds();
    thumbnailAndGroupArea.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    buttonControlArea.setBounds(bounds);
}
