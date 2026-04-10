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
#include "MidiMapping.h"
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
    CustomButton tempoDownButton{ "Tempo-", true };
    CustomButton tempoUpButton{ "Tempo+", true };
    juce::Label bpmReadout;
    CustomButton pitchDownButton{ "Pitch-", true };
    CustomButton pitchUpButton{ "Pitch+", true };
    juce::Label pitchReadout;
    CustomButton midiLearnButton{ "MIDI Learn", false };
    CustomButton tracksLayoutLeft{ "Layout-Left", false };
    CustomButton tracksLayoutRight{ "Layout-Right", false };

    void setEditor(OrbishAudioProcessorEditor* editor);
    void setMidiLearnActive(bool active);

private:
    OrbishAudioProcessorEditor* editor;

    void createTracksLayoutButton();
    void buttonClicked(Button*) override;
    void toggleMuteAll(bool);


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalControlArea)
};
