/*
  ==============================================================================

    ThumbnailArea.h
    Created: 6 Aug 2020 12:38:43pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ThumbnailArea  : public juce::Component
{
public:
    ThumbnailArea();
    ~ThumbnailArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThumbnailArea)
};
