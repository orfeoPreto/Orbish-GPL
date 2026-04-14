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

    // Time signature controls
    juce::Component tsLabel;  // placeholder for icon position
    juce::Image tsLabelIcon;
    CustomButton tsNumeratorButton{ "4", true };
    CustomButton tsPrimeButton{ "4", true };
    CustomButton tsDenominatorButton{ "4", true };
    juce::Label tsReadout;

    void setEditor(OrbishAudioProcessorEditor* editor);
    void setMidiLearnActive(bool active);
    void mouseDown(const MouseEvent& e) override;
    void updateTimeSigReadout();

private:
    OrbishAudioProcessorEditor* editor = nullptr;

    void createTracksLayoutButton();
    void buttonClicked(Button*) override;
    void toggleMuteAll(bool);

    int tsNumeratorIndex = 3;   // index into numerator values (1-13), default 4
    int tsPrimeIndex = 2;       // index into prime series, default 4
    int tsDenominatorIndex = 2; // index into denominator values, default 4

    static constexpr int kTsNumerators[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
    static constexpr int kTsPrimes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31 };
    static constexpr int kTsDenominators[] = { 1, 2, 4, 8, 16 };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlobalControlArea)
};
