/*
  ==============================================================================

    ModeAndNavigationControlArea.h
    Created: 3 Aug 2020 2:16:27pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ModeControlArea.h"
#include "NavigationControlArea.h"
#include "exu/Label.hpp"

//==============================================================================
/*
*/
class ModeAndNavigationControlArea  : public juce::Component
{
public:
    ModeAndNavigationControlArea();
    ~ModeAndNavigationControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    juce::Label titleLabel{ "Mode & Nav" };
    NavigationControlArea navigationControlArea{};
    ModeControlArea modeControlArea{};

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeAndNavigationControlArea)
};
