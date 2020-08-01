/*
  ==============================================================================

    InfoAndControlArea.h
    Created: 1 Aug 2020 12:47:40pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class InfoAndControlArea  : public juce::Component
{
public:
    InfoAndControlArea();
    ~InfoAndControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoAndControlArea)
};
