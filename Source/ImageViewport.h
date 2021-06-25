/*
  ==============================================================================

    ImageViewPort.h
    Created: 8 Jun 2021 10:20:31pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class ImageViewport  : public juce::Viewport
{
public:
    ImageViewport();
    ~ImageViewport() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void     viewedComponentChanged (Component *newComponent) override;
    void     visibleAreaChanged (const Rectangle< int > &newVisibleArea) override;
    
private:
    ImageComponent background;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageViewport)
};
