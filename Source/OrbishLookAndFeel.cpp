/*
  ==============================================================================

    OrbishLookAndFeel.cpp
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "OrbishLookAndFeel.h"



OrbishLookAndFeel::OrbishLookAndFeel() {
    // colours
    auto darkGrey = juce::Colour(0xff262626);
    auto lightGrey = juce::Colour(0xff707070);
    auto yellow = juce::Colour(0xfffed70f);


    // background
    setColour(juce::ResizableWindow::backgroundColourId, darkGrey);

    // text
    setColour(juce::Label::textColourId, lightGrey);
    setDefaultSansSerifTypefaceName("Bahnschrift");

    // buttons
    setColour(juce::TextButton::ColourIds::textColourOffId, lightGrey);
    setColour(juce::TextButton::ColourIds::textColourOnId, yellow);
    setColour(juce::TextButton::ColourIds::buttonColourId, darkGrey);
    setColour(juce::TextButton::ColourIds::buttonOnColourId, darkGrey);

    // tracks
    setColour(TrackComponent::ColourIds::backgroundColourId, darkGrey);
    setColour(TrackComponent::ColourIds::activeBackgroundColourId, lightGrey);
    setColour(TrackComponent::ColourIds::outlineColourId, yellow);

    // level meters
    setupDefaultMeterColours();
    setColour(FFAU::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    setColour(FFAU::LevelMeter::lmBackgroundColour, darkGrey);
    setColour(FFAU::LevelMeter::lmMeterBackgroundColour, lightGrey);

    // combo boxes
    setColour(juce::ComboBox::backgroundColourId, darkGrey);
    setColour(juce::ComboBox::arrowColourId, lightGrey);
    setColour(juce::ComboBox::textColourId, lightGrey);
    setColour(juce::ComboBox::focusedOutlineColourId, yellow);
    setColour(juce::ComboBox::buttonColourId, darkGrey);

    // menu bar
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, darkGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::textColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, yellow);
}

OrbishLookAndFeel::~OrbishLookAndFeel() {

}
