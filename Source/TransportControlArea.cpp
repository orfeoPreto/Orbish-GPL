/*
  ==============================================================================

    TransportControlArea.cpp
    Created: 3 Aug 2020 1:54:04pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TransportControlArea.h"
#include "PluginEditor.h"

//==============================================================================
TransportControlArea::TransportControlArea(){
    activeLabel.setText("Active Track", NotificationType::dontSendNotification);
    addAndMakeVisible(activeLabel);

    recordButton.addListener(this);
    recordButton.setTooltip("Start/Stop recording on the active track");
    recordButton.setIcon(ImageFileFormat::loadFrom(BinaryData::recordicon_png, BinaryData::recordicon_pngSize));
    addAndMakeVisible(recordButton);
//    setOpaque(true);
    playButton.addListener(this);
    playButton.setTooltip("Play/Pause the recorded material on the active track or group");
    playButton.setIcon(ImageFileFormat::loadFrom(BinaryData::playicon_png, BinaryData::playicon_pngSize));
    addAndMakeVisible(playButton);

    stopButton.addListener(this);
    stopButton.setTooltip("Stop playing on the active track or group");
    stopButton.setIcon(ImageFileFormat::loadFrom(BinaryData::stopicon_png, BinaryData::stopicon_pngSize));
    addAndMakeVisible(stopButton);

    clearButton.addListener(this);
    clearButton.setTooltip("Clear the active track");
    clearButton.setIcon(ImageFileFormat::loadFrom(BinaryData::clearicon_png, BinaryData::clearicon_pngSize));
    addAndMakeVisible(clearButton);

    muteButton.addListener(this);
    muteButton.setTooltip("Mute the active track or group");
    muteButton.setIcon(ImageFileFormat::loadFrom(BinaryData::muteicon_png, BinaryData::muteicon_pngSize));
    addAndMakeVisible(muteButton);

    soloButton.setTooltip("Play exclusively the active track or group");
    addAndMakeVisible(soloButton);
    
    monitorButton.addListener(this);
    monitorButton.setTooltip("Listen to input audio when this track is active");
    addAndMakeVisible(monitorButton);

    reverseButton.addListener(this);
    reverseButton.setTooltip("Reverse the audio on the active track");
    addAndMakeVisible(reverseButton);

    undoButton.addListener(this);
    undoButton.setTooltip("Undo latest overdub recording");
    undoButton.setIcon(ImageFileFormat::loadFrom(BinaryData::undoicon_png, BinaryData::undoicon_pngSize));
    addAndMakeVisible(undoButton);

    redoButton.addListener(this);
    redoButton.setTooltip("Redo latest overdub recording");
    redoButton.setIcon(ImageFileFormat::loadFrom(BinaryData::redoicon_png, BinaryData::redoicon_pngSize));
    addAndMakeVisible(redoButton);

    bounceButton.setTooltip("Flatten all overdub layers to one");
    addAndMakeVisible(bounceButton);

    autoTriggerButton.addListener(this);
    autoTriggerButton.setTooltip("Start recording when input signal exceeds the predefined threshold");
    addAndMakeVisible(autoTriggerButton);
}

TransportControlArea::~TransportControlArea(){

}

void TransportControlArea::paint (juce::Graphics&){
}

void TransportControlArea::resized(){

    auto bounds = getLocalBounds().reduced(15);
    activeLabel.setBounds(bounds.removeFromTop(15));
    
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(recordButton), juce::GridItem(playButton), juce::GridItem(stopButton), juce::GridItem(clearButton),
        juce::GridItem(muteButton), juce::GridItem(soloButton), juce::GridItem(undoButton), juce::GridItem(redoButton),
        juce::GridItem(monitorButton), juce::GridItem(reverseButton), juce::GridItem(bounceButton), juce::GridItem(autoTriggerButton),
    };
    
    grid.performLayout(bounds);
}

void TransportControlArea::buttonClicked(Button* button){
    if (editor == nullptr ){
        return;
    }
    if (button == &recordButton) {
        editor->toggleRecord();
    }
    if (button == &stopButton) {
        editor->toggleStop();
    }
    if (button == &clearButton) {
        editor->toggleClear();
    }
    if (button == &muteButton) {
    }
    if (button == &monitorButton) {
    }
    if (button == &reverseButton) {
        editor->toggleReverse();
    }
    if (button == &undoButton) {
    }
    if (button == &redoButton) {
    }
    if (button == &autoTriggerButton) {
    }
    if (button == &bounceButton){
    }
}

void TransportControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}
