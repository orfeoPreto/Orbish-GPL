/*
  ==============================================================================

    NavigationControlArea.h
    Created: 4 Aug 2020 4:24:03pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class NavigationControlArea  : public juce::Component
{
public:
    NavigationControlArea();
    ~NavigationControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NavigationControlArea)
};
