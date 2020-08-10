/*
  ==============================================================================

    OutputControlArea.h
    Created: 3 Aug 2020 2:16:53pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "exu/Label.hpp"
class OrbishAudioProcessorEditor;

//==============================================================================
/*
*/
class OutputControlArea  : public juce::Component, juce::Slider::Listener
{
public:
    OutputControlArea();
    ~OutputControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    Slider outputLevelSlider{ "Output Level" };
    exu::Label outputLevelLabel{ "Output Level" };
    Component outputSliderComp;
    FFAU::LevelMeter outputMeter{FFAU::LevelMeter::Minimal};

    void setEditor(OrbishAudioProcessorEditor*);

private:
    OrbishAudioProcessorEditor* editor;
    void sliderValueChanged(Slider*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputControlArea)
};
