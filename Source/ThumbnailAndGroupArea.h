/*
  ==============================================================================

    ThumbnailAndGroupArea.h
    Created: 3 Aug 2020 2:13:15pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThumbnailAndGroupArea)
};
