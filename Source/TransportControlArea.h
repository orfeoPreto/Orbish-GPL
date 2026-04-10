/*
  ==============================================================================

    TransportControlArea.h
    Created: 3 Aug 2020 1:54:04pm
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
class TransportControlArea  : public juce::Component, public Button::Listener{
public:
    TransportControlArea();
    ~TransportControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(Button* button) override;

    CustomButton recordButton{ "Record", false };
    CustomButton playButton{ "Play", false };
    CustomButton stopButton{ "Stop", true };
    CustomButton clearButton{ "Clear", true };
    CustomButton muteButton{ "Mute", false };
    CustomButton soloButton{ "Solo", false };
    CustomButton monitorButton{ "Monitor", false };
    CustomButton reverseButton{ "Revrs", false };
    CustomButton undoButton{ "Undo", true };
    CustomButton redoButton{ "Redo", true };
    CustomButton bounceButton{ "Bounce", true };
    CustomButton autoTriggerButton{ "Trigger", false };

    juce::Label activeLabel{ "Active Track" };
    juce::Label infoLabel;

    void setEditor(OrbishAudioProcessorEditor* editor);
    void updateInfo(int trackIdx, int loopIdx, int layerIdx, const juce::String& groupName);

private:
    OrbishAudioProcessorEditor* editor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportControlArea)
};
