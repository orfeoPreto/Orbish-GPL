/*
  ==============================================================================

    OutputControlArea.h
    Created: 3 Aug 2020 2:17:05pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "exu/Label.hpp"
#include "OrbishSlider.h"
#include "OrbishLevelMeter.h"

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

    OrbishSlider outputLevelSlider{ "Output Level" };
    exu::Label outputLevelLabel{ "Output Level" };
    Component outputSliderComp;
    OrbishLevelMeter outputMeter{ FFAU::LevelMeter::Minimal };

    OrbishSlider globalVolumeSlider{ "Global Volume" };
    exu::Label globalVolumeLabel{ "Global Volume" };

    void setEditor(OrbishAudioProcessorEditor*);

private:
    OrbishAudioProcessorEditor* editor;
    void sliderValueChanged(Slider*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputControlArea)
};
