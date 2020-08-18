/*
  ==============================================================================

    CustomButton.cpp
    Created: 28 Jul 2020 12:37:37pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "CustomButton.h"

CustomButton::CustomButton(const String& name, bool isPushButton) : TextButton(name) {
    pushButton = isPushButton;
    if (pushButton) {
        setColour(TextButton::ColourIds::textColourOnId, juce::Colour(0xff707070));
    }

    setToggleState(false, NotificationType::sendNotification);
    setClickingTogglesState(true);
}

CustomButton::CustomButton(const String& name, bool isPushButton, bool isSquareButton) : TextButton(name) {
    pushButton = isPushButton;
    squareButton = isSquareButton;
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

bool CustomButton::isSquareButton(){
    return squareButton;
}

bool CustomButton::isIconButton(){
    return iconButton;
}

void CustomButton::setIcon(Image iconImage){
    icon = iconImage;
    iconButton = true;
}

void CustomButton::paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown){
    auto& lf = getLookAndFeel();

    lf.drawButtonBackground(g, *this,
        findColour(getToggleState() ? buttonOnColourId : buttonColourId),
        shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    if (iconButton){
        auto bounds = getLocalBounds();
        bounds.reduce(getWidth() * 3 / 10, getHeight() * 3 / 10);

        g.drawImage(icon, bounds.toFloat(), RectanglePlacement::centred, false);
    }
    else {
        lf.drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }
}
