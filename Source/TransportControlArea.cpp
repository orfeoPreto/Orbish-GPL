/*
  ==============================================================================

    TransportControlArea.cpp
    Created: 3 Aug 2020 1:54:04pm
    Author:  Aoriseth

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TransportControlArea.h"

//==============================================================================
TransportControlArea::TransportControlArea(){
    addAndMakeVisible(button);
}

TransportControlArea::~TransportControlArea()
{
}

void TransportControlArea::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (button.getBoundsInParent(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("TransportControlArea", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void TransportControlArea::resized(){

    auto bounds = getLocalBounds();
    button.setBounds(bounds.removeFromTop(20));
}
