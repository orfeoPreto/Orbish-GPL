/*
  ==============================================================================

    GroupControlArea.h
    Created: 6 Aug 2020 12:38:51pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class GroupControlArea  : public juce::Component
{
public:
    GroupControlArea();
    ~GroupControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupControlArea)
};
