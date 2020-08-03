/*
  ==============================================================================

    ControlArea.h
    Created: 1 Aug 2020 3:22:34pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

private:

    Component thumbnailAndGroupArea;
    Component buttonControlArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlArea)
};
