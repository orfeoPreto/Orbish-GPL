/*
  ==============================================================================

    TrackArea.cpp
    Created: 1 Aug 2020 12:47:32pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TrackArea.h"

//==============================================================================
TrackArea::TrackArea(){

}

TrackArea::~TrackArea(){
}

void TrackArea::paint (juce::Graphics& g){

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void TrackArea::resized(){

}
