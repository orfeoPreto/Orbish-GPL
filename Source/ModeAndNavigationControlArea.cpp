/*
  ==============================================================================

    ModeAndNavigationControlArea.cpp
    Created: 3 Aug 2020 2:16:27pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModeAndNavigationControlArea.h"

//==============================================================================
ModeAndNavigationControlArea::ModeAndNavigationControlArea(){
    titleLabel.setText("MODE & NAV", juce::NotificationType::dontSendNotification);
    titleLabel.setFont(juce::Font(13.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffd4af37));
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(modeControlArea);
    addAndMakeVisible(navigationControlArea);
}

ModeAndNavigationControlArea::~ModeAndNavigationControlArea(){
}

void ModeAndNavigationControlArea::paint (juce::Graphics& g){
}

void ModeAndNavigationControlArea::resized(){
    auto bounds = getLocalBounds().reduced(14, 12);

    auto headerRow = bounds.removeFromTop(20);
    titleLabel.setBounds(headerRow);
    bounds.removeFromTop(6);

    modeControlArea.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
    navigationControlArea.setBounds(bounds);
}
