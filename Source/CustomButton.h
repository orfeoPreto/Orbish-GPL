/*
 ==============================================================================
 
 CustomButton.h
 Created: 28 Jul 2020 12:37:37pm
 Author:  Lennart Cockx
 
 ==============================================================================
 */

#pragma once

#include "JuceHeader.h"
class OrbishAudioProcessorEditor;

class CustomButton : public juce::TextButton {
public:
    CustomButton(const String&, bool, bool);
    CustomButton(const String&, bool);
    CustomButton();

    ~CustomButton();

    bool isPushButton();
    bool isSquareButton();
    bool isIconButton();

    void setIcon(Image icon);
    void setIconOff(Image icon);

    void paintButton(Graphics&, bool, bool) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;

    // MIDI Learn mode: when set, left-clicks are intercepted. Return true to consume.
    static std::function<bool(CustomButton*)> midiLearnClickHandler;
    // When non-null, this button is highlighted as the learn target
    static CustomButton* midiLearnHighlightedButton;
    // Per-button flag to exempt from learn mode blocking (e.g. settings close)
    bool midiLearnExcluded = false;

private:
    bool pushButton = false;
    bool squareButton = false;
    bool iconButton = false;
    Image icon;
    Image iconOff;
    bool hasIconOff = false;
};
