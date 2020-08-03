/*
  ==============================================================================

    GlobalControlArea.h
    Created: 3 Aug 2020 2:16:39pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GlobalControlArea  : public juce::Component
{
public:
    GlobalControlArea();
    ~GlobalControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalControlArea)
};
