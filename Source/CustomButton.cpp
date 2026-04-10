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
    setToggleState(false, NotificationType::sendNotification);
    setClickingTogglesState(true);
}

CustomButton::CustomButton(const String& name, bool isPushButton, bool isSquareButton) : TextButton(name) {
    pushButton = isPushButton;
    squareButton = isSquareButton;
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

void CustomButton::setIconOff(Image iconImage){
    iconOff = iconImage;
    hasIconOff = true;
}

void CustomButton::paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown){
    auto& lf = getLookAndFeel();

    lf.drawButtonBackground(g, *this,
        findColour(getToggleState() ? buttonOnColourId : buttonColourId),
        shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

    if (iconButton){
        auto bounds = getLocalBounds();
        bounds.reduce(getWidth() * 3 / 10, getHeight() * 3 / 10);

        auto accent = findColour(TextButton::ColourIds::buttonOnColourId);
        auto text   = findColour(TextButton::ColourIds::textColourOffId);
        bool active = !isPushButton() && getToggleState();
        auto& img = (hasIconOff && !active) ? iconOff : icon;

        if (shouldDrawButtonAsDown || active){
            g.setColour(accent);
            g.drawImage(img, bounds.toFloat(), RectanglePlacement::centred, true);
        }
        else if(shouldDrawButtonAsHighlighted) {
            g.setColour(text.brighter(0.3f));
            g.drawImage(img, bounds.toFloat(), RectanglePlacement::centred, true);
        }
        else {
            g.setColour(text.withAlpha(0.7f));
            g.drawImage(img, bounds.toFloat(), RectanglePlacement::centred, true);
        }
    }
    else {
        lf.drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }
}
