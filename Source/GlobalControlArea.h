/*
  ==============================================================================

    GlobalControlArea.h
    Created: 3 Aug 2020 2:16:39pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomButton.h"
#include "exu/Label.hpp"
class OrbishAudioProcessorEditor;

//==============================================================================
/*
*/
class GlobalControlArea  : public juce::Component, juce::Button::Listener {
public:
    GlobalControlArea();
    ~GlobalControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    exu::Label globalLabel{ "Global" };
    CustomButton muteAllButton{ "Mute", false};
    CustomButton stopAllButton{ "Stop", true};
    CustomButton startAllButton{ "Start", true};
    CustomButton pauseAllButton{ "Pause", false};
    CustomButton clearAllButton{ "Clear", true};
    DrawableButton tracksLayoutLeft{"Layout-Left", DrawableButton::ImageStretched };
    DrawableButton tracksLayoutRight{ "Layout-Right", DrawableButton::ImageStretched };

    void setEditor(OrbishAudioProcessorEditor* editor);

private:
    OrbishAudioProcessorEditor* editor;

    void createTracksLayoutButton();
    void buttonClicked(Button*) override;
    void toggleMuteAll(bool);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalControlArea)
};
