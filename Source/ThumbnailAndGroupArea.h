/*
  ==============================================================================

    ThumbnailAndGroupArea.h
    Created: 3 Aug 2020 2:13:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ThumbnailArea.h"
#include "GroupControlArea.h"

//==============================================================================
/*
*/
class ThumbnailAndGroupArea  : public juce::Component
{
public:
    ThumbnailAndGroupArea();
    ~ThumbnailAndGroupArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void updateTimeDisplay(const juce::String& positionTime, const juce::String& totalTime);

    ThumbnailArea thumbnailArea{};

private:
    juce::Label timeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThumbnailAndGroupArea)
};
