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
}

void InfoAndControlArea::resized(){
    auto bounds = getLocalBounds();
    infoArea.setBounds(bounds.removeFromLeft(150));
    controlArea.setBounds(bounds);
}
