/*
  ==============================================================================

    TrackArea.h
    Created: 1 Aug 2020 12:47:32pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TrackArea  : public juce::Component
{
public:
    TrackArea();
    ~TrackArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackArea)
};
