/*
  ==============================================================================

    InputControlArea.cpp
    Created: 3 Aug 2020 2:17:05pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InputControlArea.h"
#include "PluginEditor.h"

//==============================================================================
InputControlArea::InputControlArea(){

    // input slider/meter
    inputLevelLabel.setText("Input Level", NotificationType::dontSendNotification);
    inputLevelLabel.attachToComponent(&inputLevelSlider, false);
    addAndMakeVisible(inputLevelLabel);

    inputLevelSlider.setNumDecimalPlacesToDisplay(1);
    inputLevelSlider.setTextBoxIsEditable(true);
    inputLevelSlider.setTextValueSuffix(" db");
    inputLevelSlider.addListener(this);
    inputLevelSlider.setSliderStyle(Slider::LinearBarVertical);
    inputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    inputLevelSlider.setPopupDisplayEnabled(true, false, this);
    inputLevelSlider.setTooltip("Adjust the level of the input signal for the active track");
    inputLevelSlider.textFromValueFunction = [this](double val){return String(val, 1);};
    addAndMakeVisible(inputLevelSlider);

    addAndMakeVisible(inputMeter);

    // global slider
    globalVolumeLabel.setText("Global Level", NotificationType::dontSendNotification);
    globalVolumeLabel.attachToComponent(&globalVolumeSlider, false);
    addAndMakeVisible(globalVolumeLabel);

    globalVolumeSlider.setValue(0);
    globalVolumeSlider.setRange(-120, 6);
    globalVolumeSlider.setNumDecimalPlacesToDisplay(2);
    globalVolumeSlider.setTextBoxIsEditable(true);
    globalVolumeSlider.setTextValueSuffix(" db");
    globalVolumeSlider.addListener(this);
    globalVolumeSlider.setSliderStyle(Slider::LinearBarVertical);
    globalVolumeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    globalVolumeSlider.setPopupDisplayEnabled(true, false, this);
    globalVolumeSlider.textFromValueFunction = [this](double val){ return String(val, 1);};
    globalVolumeSlider.setTooltip("Adjust the level of the general output signal");
    addAndMakeVisible(globalVolumeSlider);

}

InputControlArea::~InputControlArea(){
    inputMeter.setLookAndFeel(nullptr);
}

void InputControlArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle(getLocalBounds().reduced(5).toFloat(), 4.0f, 1.0f);   // draw an outline around the component
}

void InputControlArea::resized(){
    auto bounds = getLocalBounds().reduced(15);

    inputLevelLabel.setBounds(bounds.removeFromTop(15));
    globalVolumeSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 3));
    bounds.removeFromLeft(5);
    inputLevelSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    bounds.removeFromLeft(5);
    inputMeter.setBounds(bounds);
}

void InputControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
    inputMeter.setMeterSource(editor->getProcessor()->getInputMeterSource());

}

void InputControlArea::sliderValueChanged(Slider* slider){
    if (slider == &inputLevelSlider) {
    }
    if (slider == &globalVolumeSlider){
    }
}
