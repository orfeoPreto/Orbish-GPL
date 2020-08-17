/*
  ==============================================================================

    ThumbnailArea.h
    Created: 6 Aug 2020 12:38:43pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class OrbishAudioProcessorEditor;

//==============================================================================
/*
*/
class ThumbnailArea  : public juce::Component, private juce::Timer
{
public:
    ThumbnailArea();
    ~ThumbnailArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    Rectangle<int> playHead;

    void setFileLoaded(bool);
    void setEditor(OrbishAudioProcessorEditor*);
    AudioVisualiserComponent inputDisplay{ 2 };

    void updatePlayHead(int);

private:

    OrbishAudioProcessorEditor* editor;
    bool fileLoaded = false;
    float playHeadPosition = 0;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThumbnailArea)
};
