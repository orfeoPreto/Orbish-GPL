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

class OrbishLookAndFeel : public juce::LookAndFeel_V3, public FFAU::LevelMeter::LookAndFeelMethods
{
public:
    OrbishLookAndFeel();
    ~OrbishLookAndFeel();

    #include "ff_meters/ff_meters_LookAndFeelMethods.h"
private:
};