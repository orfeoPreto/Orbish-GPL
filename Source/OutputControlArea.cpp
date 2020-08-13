/*
  ==============================================================================

    OutputControlArea.cpp
    Created: 3 Aug 2020 2:16:53pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OutputControlArea.h"
#include "PluginEditor.h"

//==============================================================================
OutputControlArea::OutputControlArea(){
    addAndMakeVisible(outputMeter);

    outputLevelLabel.setText("Output Level", NotificationType::dontSendNotification);
    outputLevelLabel.attachToComponent(&outputLevelSlider, false);
    addAndMakeVisible(outputLevelLabel);

    outputLevelSlider.setRange(-60, 6);
    outputLevelSlider.setNumDecimalPlacesToDisplay(1);
    outputLevelSlider.setTextBoxIsEditable(true);
    outputLevelSlider.setTextValueSuffix(" db");
    outputLevelSlider.addListener(this);
    outputLevelSlider.textFromValueFunction = [this](double val){ return String(val, 1);};
    outputLevelSlider.setTooltip("Adjust the level of the output signal for the active track");
    outputLevelSlider.setSliderStyle(Slider::LinearBarVertical);
    outputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    outputLevelSlider.setPopupDisplayEnabled(true, false, this);
    addAndMakeVisible(outputLevelSlider);
}

OutputControlArea::~OutputControlArea(){
    outputMeter.setLookAndFeel(nullptr);
}

void OutputControlArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle(getLocalBounds().reduced(5).toFloat(), 4.0f, 1.0f);   // draw an outline around the component
}

void OutputControlArea::resized(){
    auto bounds = getLocalBounds().reduced(15);

    outputLevelLabel.setBounds(bounds.removeFromTop(15));
    outputLevelSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    bounds.removeFromLeft(5);
    outputMeter.setBounds(bounds);
}

void OutputControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
    outputMeter.setMeterSource(editor->getProcessor()->getOutputMeterSource());
    
}

void OutputControlArea::sliderValueChanged(Slider* slider){
    if (slider == &outputLevelSlider) {
    }
}
