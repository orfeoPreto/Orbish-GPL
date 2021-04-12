/*
  ==============================================================================

    InputControlArea.h
    Created: 3 Aug 2020 2:16:53pm
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
class InputControlArea  : public juce::Component, juce::Slider::Listener
{
public:
    InputControlArea();
    ~InputControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    OrbishLevelMeter inputMeter{ FFAU::LevelMeter::Minimal };
    OrbishSlider inputLevelSlider{ "Input Level" };
    exu::Label inputLevelLabel{ "Input Level" };

    void setEditor(OrbishAudioProcessorEditor*);

private:
    OrbishAudioProcessorEditor* editor;
    void sliderValueChanged(Slider*) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputControlArea)
};
