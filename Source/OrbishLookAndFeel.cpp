/*
  ==============================================================================

    OrbishLookAndFeel.cpp
    Created: 23 Jul 2020 12:22:54pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include "OrbishLookAndFeel.h"



OrbishLookAndFeel::OrbishLookAndFeel() {
    // colours
    auto darkGrey = juce::Colour(0xff262626);
    auto lightGrey = juce::Colour(0xff707070);
    auto yellow = juce::Colour(0xfffed70f);


    // background
    setColour(juce::ResizableWindow::backgroundColourId, darkGrey);

    // text
    setColour(juce::Label::textColourId, lightGrey);
    setDefaultSansSerifTypefaceName("Bahnschrift");

    // buttons
    setColour(juce::TextButton::ColourIds::textColourOffId, lightGrey);
    setColour(juce::TextButton::ColourIds::textColourOnId, yellow);
    setColour(juce::TextButton::ColourIds::buttonColourId, darkGrey);
    setColour(juce::TextButton::ColourIds::buttonOnColourId, yellow);

    setColour(DrawableButton::ColourIds::backgroundColourId, darkGrey);
    setColour(DrawableButton::ColourIds::backgroundOnColourId, darkGrey);

    // tracks
    setColour(TrackComponent::ColourIds::backgroundColourId, darkGrey);
    setColour(TrackComponent::ColourIds::activeBackgroundColourId, lightGrey);
    setColour(TrackComponent::ColourIds::outlineColourId, yellow);

    // level meters
    setupDefaultMeterColours();
    setColour(FFAU::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    setColour(FFAU::LevelMeter::lmBackgroundColour, darkGrey);
    setColour(FFAU::LevelMeter::lmMeterBackgroundColour, lightGrey);

    // combo boxes
    setColour(juce::ComboBox::backgroundColourId, darkGrey);
    setColour(juce::ComboBox::arrowColourId, lightGrey);
    setColour(juce::ComboBox::textColourId, lightGrey);
    setColour(juce::ComboBox::focusedOutlineColourId, yellow);
    setColour(juce::ComboBox::buttonColourId, darkGrey);

    // menu bar
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, darkGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::textColourId, lightGrey);
    setColour(juce::PopupMenu::ColourIds::highlightedTextColourId, yellow);

    // sliders
    setColour(juce::Slider::ColourIds::backgroundColourId, darkGrey);
    setColour(juce::Slider::ColourIds::trackColourId, lightGrey);
    setColour(juce::Slider::ColourIds::thumbColourId, yellow);

    // progress bars
    setColour(juce::ProgressBar::ColourIds::backgroundColourId, lightGrey);
    setColour(juce::ProgressBar::ColourIds::foregroundColourId, yellow);

    // scroll bars
    setColour(juce::ScrollBar::ColourIds::trackColourId, yellow);
    setColour(juce::ScrollBar::ColourIds::thumbColourId, darkGrey);
    setColour(juce::ScrollBar::ColourIds::backgroundColourId, lightGrey);
}

OrbishLookAndFeel::~OrbishLookAndFeel() {

}



void OrbishLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown) {
    g.setColour(juce::Colour(0xff262626));
    g.fillRect(button.getLocalBounds());

    auto buttonShape = isSquareButton(&button) ? ButtonShape::SQUARE : ButtonShape::RECTANGULAR;

    if (isHovering){
        button.setColour(TextButton::textColourOffId, Colours::whitesmoke);
        button.setColour(TextButton::textColourOnId, Colours::whitesmoke);
    }
    else {
        button.setColour(TextButton::textColourOffId, findColour(TextButton::textColourOffId));
        button.setColour(TextButton::textColourOnId, findColour(TextButton::textColourOnId));
    }

    if (isPushButton(&button)){
        if (!isHovering){
            button.setColour(TextButton::textColourOnId, findColour(TextButton::textColourOffId));
        }
        drawPushButton(g, button, backgroundColour, isHovering, isButtonDown, buttonShape);
    }
    else{
        drawToggleButton(g, button, backgroundColour, isHovering, isButtonDown, buttonShape);
    }
}

void OrbishLookAndFeel::drawPushButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape){
    Image image;
    if (isButtonDown) {
        image = getImageForButton(shape, ButtonState::CLICKED);
    }
    else if (isHovering) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else {
        image = getImageForButton(shape, ButtonState::BASE);
    }
    auto bounds = button.getLocalBounds();
    g.drawImageWithin(image, 0, 0, bounds.getWidth(), bounds.getHeight(), RectanglePlacement(), false);
}

void OrbishLookAndFeel::drawToggleButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown, ButtonShape shape) {
    Image image;
    if ((button.getToggleState() && isButtonDown)) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else if (isButtonDown || (button.getToggleState() && isHovering)) {
        image = getImageForButton(shape, ButtonState::CLICKED);
    }
    else if (isHovering) {
        image = getImageForButton(shape, ButtonState::HOVERING);
    }
    else if (button.getToggleState()) {
        image = getImageForButton(shape, ButtonState::ACTIVE);
    }
    else {
        image = getImageForButton(shape, ButtonState::BASE);
    }
    auto bounds = button.getLocalBounds();
    g.drawImageWithin(image, 0, 0, bounds.getWidth(), bounds.getHeight(), RectanglePlacement(), false);
}

bool OrbishLookAndFeel::isPushButton(juce::Button* button){
    if (auto orbishButton = dynamic_cast<CustomButton*>(button)) {
        if (orbishButton->isPushButton()) {
            return true;
        }
    }
    return false;
}

bool OrbishLookAndFeel::isSquareButton(juce::Button* button) {
    if (auto orbishButton = dynamic_cast<CustomButton*>(button)) {
        if (orbishButton->isSquareButton()) {
            return true;
        }
    }
    return false;
}

Image OrbishLookAndFeel::getImageForButton(ButtonShape shape, ButtonState state){
    if (shape == ButtonShape::SQUARE){
        switch (state) {
            case ButtonState::BASE: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbase_png, BinaryData::squarebuttonbase_pngSize);
            case ButtonState::HOVERING: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbasehover_png, BinaryData::squarebuttonbasehover_pngSize);
            case ButtonState::CLICKED: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbaseclicked_png, BinaryData::squarebuttonbaseclicked_pngSize);
            case ButtonState::ACTIVE: return ImageFileFormat::loadFrom(BinaryData::squarebuttonbaseactive_png, BinaryData::squarebuttonbaseactive_pngSize);
        }
    }
    else{
        switch (state) {
        case ButtonState::BASE: return ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
        case ButtonState::HOVERING: return ImageFileFormat::loadFrom(BinaryData::buttonbasehover_png, BinaryData::buttonbasehover_pngSize);
        case ButtonState::CLICKED: return ImageFileFormat::loadFrom(BinaryData::buttonbaseclicked_png, BinaryData::buttonbaseclicked_pngSize);
        case ButtonState::ACTIVE: return ImageFileFormat::loadFrom(BinaryData::buttonbaseactive_png, BinaryData::buttonbaseactive_pngSize);
        }
    }
    return ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
}
