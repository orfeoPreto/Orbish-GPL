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

    setPrimaryColour(darkGrey);
    setSecondaryColour(lightGrey);
    setAccentColour(yellow);


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
}

OrbishLookAndFeel::~OrbishLookAndFeel() {

}

Colour OrbishLookAndFeel::getPrimaryColour()
{
    return primaryColour;
}

Colour OrbishLookAndFeel::getSecondaryColour()
{
    return secondaryColour;
}

Colour OrbishLookAndFeel::getAccentColour()
{
    return accentColour;
}

void OrbishLookAndFeel::setPrimaryColour(Colour& colour)
{
    primaryColour = colour;
}

void OrbishLookAndFeel::setSecondaryColour(Colour& colour)
{
    secondaryColour = colour;
}

void OrbishLookAndFeel::setAccentColour(Colour& colour)
{
    accentColour = colour;
}
