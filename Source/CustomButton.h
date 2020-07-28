/*
  ==============================================================================

    CustomButton.h
    Created: 28 Jul 2020 12:37:37pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class CustomButton : public juce::TextButton {
public:
    CustomButton(const String&, bool, juce::Button::Listener*);
    ~CustomButton();

    bool isPushButton();

private:
    bool pushButton;
};