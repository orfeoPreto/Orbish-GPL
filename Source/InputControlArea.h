/*
  ==============================================================================

    InputControlArea.h
    Created: 3 Aug 2020 2:17:05pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class InputControlArea  : public juce::Component
{
public:
    InputControlArea();
    ~InputControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputControlArea)
};
