/*
  ==============================================================================

    ModeAndNavigationControlArea.h
    Created: 3 Aug 2020 2:16:27pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModeAndNavigationControlArea)
};
