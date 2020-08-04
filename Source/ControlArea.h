/*
  ==============================================================================

    ControlArea.h
    Created: 1 Aug 2020 3:22:34pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ButtonControlArea.h"
#include "ThumbnailAndGroupArea.h"

//==============================================================================
/*
*/
class ControlArea  : public juce::Component
{
public:
    ControlArea();
    ~ControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    ButtonControlArea buttonControlArea{};

private:

    ThumbnailAndGroupArea thumbnailAndGroupArea{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlArea)
};
