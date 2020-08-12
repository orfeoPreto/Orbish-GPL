/*
  ==============================================================================

    TrackArea.h
    Created: 1 Aug 2020 12:47:32pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <Source\Track.h>
#include <Source\TrackComponent.h>

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
