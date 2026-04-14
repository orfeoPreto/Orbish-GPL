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
    activeLabel.setText("ACTIVE TRACK", NotificationType::dontSendNotification);
    activeLabel.setFont(juce::Font(15.0f, juce::Font::bold));
    activeLabel.setColour(juce::Label::textColourId, juce::Colour(0xffd4af37));
    addAndMakeVisible(activeLabel);

    infoLabel.setFont(juce::Font(11.0f, juce::Font::plain));
    infoLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.55f));
    infoLabel.setJustificationType(juce::Justification::centredRight);
    infoLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(infoLabel);

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
    monitorButton.setIcon(ImageFileFormat::loadFrom(BinaryData::monitoricon_png, BinaryData::monitoricon_pngSize));
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

void TransportControlArea::paint (juce::Graphics& g){
}

void TransportControlArea::resized(){

    auto bounds = getLocalBounds().reduced(14, 12);

    // Title row
    activeLabel.setBounds(bounds.removeFromTop(20));
    bounds.removeFromTop(4);
    // Info row below title
    infoLabel.setBounds(bounds.removeFromTop(16));
    // Gap to align first button row with record mode combo in Mode & Nav
    bounds.removeFromTop(22);

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    using Px = juce::Grid::Px;

    grid.rowGap = Px(4);
    grid.columnGap = Px(4);

    // Match nav button height (~Fr based, same as loop prev/next)
    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        // Row A: primary transport
        juce::GridItem(recordButton), juce::GridItem(playButton), juce::GridItem(stopButton),
        juce::GridItem(clearButton).withArea({}, juce::GridItem::Span(2)),
        // Row B: secondary state + undo/redo
        juce::GridItem(muteButton), juce::GridItem(soloButton), juce::GridItem(monitorButton),
        juce::GridItem(undoButton), juce::GridItem(redoButton),
        // Row C: effects
        juce::GridItem(reverseButton), juce::GridItem(bounceButton), juce::GridItem(autoTriggerButton),
        juce::GridItem(), juce::GridItem()
    };

    grid.performLayout(bounds);
}

void TransportControlArea::buttonClicked(Button* button){
    if (editor == nullptr) return;

    // In MIDI learn mode, block all normal button actions
    if (editor->isMidiLearnActive()) {
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

void TransportControlArea::updateInfo(int trackIdx, int loopIdx, int layerIdx, const juce::String& groupName) {
    auto text = "Track:" + String(trackIdx) + "  Loop:" + String(loopIdx) + "  Layer:" + String(layerIdx);
    if (groupName.isNotEmpty())
        text += "  Group:" + groupName;
    if (infoLabel.getText() != text)
        infoLabel.setText(text, NotificationType::dontSendNotification);
}
