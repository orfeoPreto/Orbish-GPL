/*
  ==============================================================================

    CustomButton.cpp
    Created: 28 Jul 2020 12:37:37pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "CustomButton.h"

CustomButton::CustomButton(const String& name, bool isPushButton, juce::Button::Listener* listener): TextButton (name){
    this->pushButton = isPushButton;
    if (pushButton){
        setColour(TextButton::ColourIds::textColourOnId, juce::Colour(0xff707070));
    }

    setToggleState(false, NotificationType::sendNotification);
    setClickingTogglesState(true);
    addListener(listener);
}

CustomButton::~CustomButton(){
}

bool CustomButton::isPushButton(){
    return pushButton;
}
