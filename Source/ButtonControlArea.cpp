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
    auto bounds = transportControlArea.getBoundsInParent();
    bounds.enlargeIfAdjacent(modeAndNavigationControlArea.getBoundsInParent());
    bounds.enlargeIfAdjacent(globalControlArea.getBoundsInParent());

    g.setColour(Colours::black);
    g.drawRoundedRectangle(bounds.reduced(5).toFloat(), 4.0f, 1.0f);
}

void ButtonControlArea::resized(){
    auto bounds = getLocalBounds();

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows = { Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(9)), Track(Fr(6)), Track(Fr(5)), Track(Fr(2)), Track(Fr(3)) };

    grid.items = {
        juce::GridItem(transportControlArea), juce::GridItem(modeAndNavigationControlArea), juce::GridItem(globalControlArea), juce::GridItem(inputControlArea), juce::GridItem(outputControlArea)
    };

    grid.performLayout(bounds);
}
