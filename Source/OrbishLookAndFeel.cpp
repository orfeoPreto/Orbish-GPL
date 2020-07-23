/*
  ==============================================================================

    OrbishLookAndFeel.cpp
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "OrbishLookAndFeel.h"

OrbishLookAndFeel::OrbishLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::black);
    setColour(juce::Label::textColourId, juce::Colours::white);
}

OrbishLookAndFeel::~OrbishLookAndFeel() {

}