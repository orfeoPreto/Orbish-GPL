/*
  ==============================================================================

    InputControlArea.cpp
    Created: 3 Aug 2020 2:16:53pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InputControlArea.h"
#include "PluginEditor.h"

//==============================================================================
InputControlArea::InputControlArea(){
    

    // input slider/meter
    inputLevelLabel.setText("Input", NotificationType::dontSendNotification);
    inputLevelLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(inputLevelLabel);

    inputLevelSlider.setNumDecimalPlacesToDisplay(1);
    inputLevelSlider.setTextBoxIsEditable(true);
    inputLevelSlider.setTextValueSuffix(" db");
    inputLevelSlider.addListener(this);
    inputLevelSlider.setSliderStyle(Slider::LinearBarVertical);
    inputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    inputLevelSlider.setPopupDisplayEnabled(true, false, this);
    inputLevelSlider.setTooltip("Adjust the level of the input signal for the active track");
   // inputLevelSlider.textFromValueFunction = [this](double val) {return String(val, 1); };
    addAndMakeVisible(inputLevelSlider);

    addAndMakeVisible(inputMeter);
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

    inputLevelLabel.setBounds(bounds.removeFromBottom(20));
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
}
