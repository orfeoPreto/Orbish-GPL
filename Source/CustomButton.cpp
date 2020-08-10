/*
  ==============================================================================

    CustomButton.cpp
    Created: 28 Jul 2020 12:37:37pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "CustomButton.h"
#include "PluginEditor.h"

CustomButton::CustomButton(const String& name, bool isPushButton, OrbishAudioProcessorEditor* pluginEditor) : TextButton(name) {
    this->addListener(pluginEditor);
    this->pushButton = isPushButton;
    if (pushButton) {
        setColour(TextButton::ColourIds::textColourOnId, juce::Colour(0xff707070));
    }

    setToggleState(false, NotificationType::sendNotification);
    setClickingTogglesState(true);
}

CustomButton::CustomButton(const String& name, bool isPushButton): TextButton (name){
    this->pushButton = isPushButton;
    if (pushButton) {
        setColour(TextButton::ColourIds::textColourOnId, juce::Colour(0xff707070));
    }

    setToggleState(false, NotificationType::sendNotification);
    setClickingTogglesState(true);
}

CustomButton::~CustomButton(){
}

bool CustomButton::isPushButton(){
    return pushButton;
}
