/*
  ==============================================================================

    OutputControlArea.h
    Created: 3 Aug 2020 2:16:53pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OutputControlArea  : public juce::Component
{
public:
    OutputControlArea();
    ~OutputControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputControlArea)
};
