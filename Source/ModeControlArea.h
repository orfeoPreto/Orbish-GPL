/*
  ==============================================================================

    ModeControlArea.h
    Created: 4 Aug 2020 4:24:10pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "exu/Label.hpp"

//==============================================================================
/*
*/
class ModeControlArea  : public juce::Component
{
public:
    ModeControlArea();
    ~ModeControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    ComboBox recModeCombo;
    ComboBox snapModeCombo;

    exu::Label recModeLabel = { {}, {"Record Mode"} };
    exu::Label snapModeLabel = { {}, {"Snap Mode"} };


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeControlArea)
};
