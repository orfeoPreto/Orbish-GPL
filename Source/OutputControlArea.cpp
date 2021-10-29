/*
  ==============================================================================

    OutputControlArea.cpp
    Created: 3 Aug 2020 2:17:05pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OutputControlArea.h"
#include "PluginEditor.h"

//==============================================================================
OutputControlArea::OutputControlArea(){
    addAndMakeVisible(outputMeter);
    
    // output slider
    outputLevelLabel.setText("Output", NotificationType::dontSendNotification);
    outputLevelLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(outputLevelLabel);

    outputLevelSlider.setRange(-60, 12);
    outputLevelSlider.setNumDecimalPlacesToDisplay(1);
    outputLevelSlider.setTextBoxIsEditable(true);
    outputLevelSlider.setTextValueSuffix(" db");
    outputLevelSlider.addListener(this);
    outputLevelSlider.textFromValueFunction = [this](double val) { return String(val, 1); };
    outputLevelSlider.setTooltip("Adjust the level of the output signal for the active track");
    outputLevelSlider.setSliderStyle(Slider::LinearBarVertical);
    outputLevelSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    outputLevelSlider.setPopupDisplayEnabled(true, false, this);
    outputLevelSlider.setDoubleClickReturnValue(true, 0);
    addAndMakeVisible(outputLevelSlider);

    // global slider
    globalVolumeLabel.setText("Global Level", NotificationType::dontSendNotification); //todo correctly position label
    addAndMakeVisible(globalVolumeLabel);

    globalVolumeSlider.setValue(0);
    globalVolumeSlider.setRange(-60, 12);
    globalVolumeSlider.setNumDecimalPlacesToDisplay(2);
    globalVolumeSlider.setTextBoxIsEditable(true);
    globalVolumeSlider.setTextValueSuffix(" db");
    globalVolumeSlider.addListener(this);
    globalVolumeSlider.setSliderStyle(Slider::LinearBarVertical);
    globalVolumeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    globalVolumeSlider.setPopupDisplayEnabled(true, false, this);
    globalVolumeSlider.textFromValueFunction = [this](double val){ return String(val, 1);};
    globalVolumeSlider.setTooltip("Adjust the level of the general output signal");
    globalVolumeSlider.setDoubleClickReturnValue(true, 0);
//    globalVolumeSlider.setSkewFactorFromMidPoint(2.5);
    addAndMakeVisible(globalVolumeSlider);

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

    outputLevelLabel.setBounds(bounds.removeFromBottom(20));
    outputLevelSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 3));
//    outputLevelSlider.setCentrePosition(0, outputLevelSlider.getHeight() * .5f);

    bounds.removeFromLeft(5);
    globalVolumeSlider.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    bounds.removeFromLeft(5);
    outputMeter.setBounds(bounds);
}

void OutputControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
   outputMeter.setMeterSource(editor->getProcessor()->getOutputMeterSource());
    outputMeter.setOpenGLContext(editor->getOpenGLContext());
//    outputMeter.meterDisplay->setTopLevelComponent(editor);

}

void OutputControlArea::sliderValueChanged(Slider* slider){
    if (slider == &outputLevelSlider) {
    }
    if (slider == &globalVolumeSlider){
    }
}
