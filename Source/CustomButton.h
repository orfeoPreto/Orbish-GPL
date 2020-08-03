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
    CustomButton(const String&, bool, OrbishAudioProcessorEditor*);
    CustomButton(const String&, bool);
    ~CustomButton();

    bool isPushButton();

private:
    bool pushButton;
};