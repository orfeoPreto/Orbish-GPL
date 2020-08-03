/*
  ==============================================================================

    ButtonControlArea.h
    Created: 3 Aug 2020 2:12:34pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportControlArea.h"
#include "ModeAndNavigationControlArea.h"
#include "GlobalControlArea.h"
#include "OutputControlArea.h"
#include "InputControlArea.h"

//==============================================================================
/*
*/
class ButtonControlArea  : public juce::Component
{
public:
    ButtonControlArea();
    ~ButtonControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TransportControlArea transportControlArea{};
    ModeAndNavigationControlArea modeAndNavigationControlArea{};
    GlobalControlArea globalControlArea{};
    OutputControlArea outputControlArea{};
    InputControlArea inputControlArea{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonControlArea)
};
