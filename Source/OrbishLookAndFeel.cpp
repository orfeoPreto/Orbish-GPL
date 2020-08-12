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

    if (isPushButton(&button)){
        drawPushButton(g, button, backgroundColour, isHovering, isButtonDown);
    }
    else{
        drawToggleButton(g, button, backgroundColour, isHovering, isButtonDown);
    }
}

void OrbishLookAndFeel::drawPushButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown){
    Image image;
    if (isButtonDown) {
        image = ImageFileFormat::loadFrom(BinaryData::buttonbaseclicked_png, BinaryData::buttonbaseclicked_pngSize);
    }
    else if (isHovering) {
        image = ImageFileFormat::loadFrom(BinaryData::buttonbasehover_png, BinaryData::buttonbasehover_pngSize);
    }
    else {
        image = ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
    }
    auto bounds = button.getLocalBounds();
    g.drawImageWithin(image, 0, 0, bounds.getWidth(), bounds.getHeight(), RectanglePlacement(), false);
}

void OrbishLookAndFeel::drawToggleButton(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool isHovering, bool isButtonDown){
    Image image;
    if ((button.getToggleState() && isButtonDown)){
        image = ImageFileFormat::loadFrom(BinaryData::buttonbasehover_png, BinaryData::buttonbasehover_pngSize);
    }
    else if (isButtonDown || (button.getToggleState() && isHovering)){
        image = ImageFileFormat::loadFrom(BinaryData::buttonbaseclicked_png, BinaryData::buttonbaseclicked_pngSize);
    }
    else if (isHovering) {
        image = ImageFileFormat::loadFrom(BinaryData::buttonbasehover_png, BinaryData::buttonbasehover_pngSize);
    }
    else if (button.getToggleState()){
        image = ImageFileFormat::loadFrom(BinaryData::buttonbaseactive_png, BinaryData::buttonbaseactive_pngSize);
    }
    else{
        image = ImageFileFormat::loadFrom(BinaryData::buttonbase_png, BinaryData::buttonbase_pngSize);
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
