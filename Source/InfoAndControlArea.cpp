/*
  ==============================================================================

    InfoAndControlArea.cpp
    Created: 1 Aug 2020 12:47:40pm
    Author:  lenna

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InfoAndControlArea.h"

//==============================================================================
InfoAndControlArea::InfoAndControlArea(){
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    addAndMakeVisible(infoArea);
    addAndMakeVisible(controlArea);

}

InfoAndControlArea::~InfoAndControlArea(){
}

void InfoAndControlArea::paint (juce::Graphics& g){

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::yellow);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("InfoAndControlArea", getLocalBounds(),juce::Justification::centred, true);   // draw some placeholder text
}

void InfoAndControlArea::resized(){
    auto bounds = getLocalBounds();
    infoArea.setBounds(bounds.removeFromLeft(juce::jmax(150, bounds.getWidth() / 5)));
    controlArea.setBounds(bounds);
}
