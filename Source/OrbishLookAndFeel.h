/*
  ==============================================================================

    OrbishLookAndFeel.h
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "TrackComponent.h"
#include "CustomButton.h"

enum class ButtonShape {
    SQUARE,
    RECTANGULAR
};

enum class ButtonState {
    BASE,
    CLICKED,
    HOVERING,
    ACTIVE
};

class OrbishLookAndFeel : public juce::LookAndFeel_V3, public FFAU::LevelMeter::LookAndFeelMethods
{
public:
    OrbishLookAndFeel();
    ~OrbishLookAndFeel();

    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool something, bool isButtonDown) override;
    #include "ff_meters/ff_meters_LookAndFeelMethods.h"
private:
    void drawPushButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape);
    void drawToggleButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape);
    bool isPushButton(juce::Button* button);
    bool isSquareButton(juce::Button* button);
    Image getImageForButton(ButtonShape shape, ButtonState state);

};


