/*
  ==============================================================================

    ButtonControlArea.h
    Created: 3 Aug 2020 2:12:34pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TransportControlArea.h"
#include "ModeAndNavigationControlArea.h"
#include "GlobalControlArea.h"
#include "OutputControlArea.h"
#include "InputControlArea.h"
#include "OpenGLComponents.h"
#include "CustomButton.h"

class GroupControlArea;

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

    // Adopt click controls from InfoArea into the utility strip
    void adoptClickControls(OpenGLClickWitness* witness, CustomButton* clickBtn);
    // Adopt group controls into the utility strip
    void adoptGroupControls(GroupControlArea* groupArea);

    TransportControlArea transportControlArea{};
    ModeAndNavigationControlArea modeAndNavigationControlArea{};
    GlobalControlArea globalControlArea{};
    OutputControlArea outputControlArea{};
    InputControlArea inputControlArea{};

private:
    // Adopted controls (owned elsewhere)
    OpenGLClickWitness* clickWitness = nullptr;
    CustomButton* clickButton = nullptr;
    GroupControlArea* groupControls = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ButtonControlArea)
};
