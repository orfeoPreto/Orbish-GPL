/*
  ==============================================================================

    InputControlArea.h
    Created: 3 Aug 2020 2:17:05pm
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
class InputControlArea  : public juce::Component, juce::Slider::Listener
{
public:
    InputControlArea();
    ~InputControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    FFAU::LevelMeter inputMeter{FFAU::LevelMeter::Minimal};
    Slider inputLevelSlider{ "Input Level" };
    exu::Label inputLevelLabel{ "Input Level" };

    Slider globalVolumeSlider{ "Global Volume" };
    exu::Label globalVolumeLabel{ "Global Volume" };

    void setEditor(OrbishAudioProcessorEditor*);

private:
    OrbishAudioProcessorEditor* editor;
    void sliderValueChanged(Slider*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputControlArea)
};
