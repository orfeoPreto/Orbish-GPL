/*
  ==============================================================================

    GlobalControlArea.cpp
    Created: 3 Aug 2020 2:16:39pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GlobalControlArea.h"
#include "PluginEditor.h"

//==============================================================================
GlobalControlArea::GlobalControlArea(){
    globalLabel.setText("Global", NotificationType::dontSendNotification);
    addAndMakeVisible(globalLabel);

    muteAllButton.setTooltip("Mute all tracks");
    muteAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::muteicon_png, BinaryData::muteicon_pngSize));
    addAndMakeVisible(muteAllButton);

    startAllButton.setTooltip("Start playing on all tracks");
    startAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::playicon_png, BinaryData::playicon_pngSize));
    addAndMakeVisible(startAllButton);

    stopAllButton.setTooltip("Stop playing on all tracks");
    stopAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::stopicon_png, BinaryData::stopicon_pngSize));
    addAndMakeVisible(stopAllButton);

    clearAllButton.setTooltip("Clear all tracks");
    clearAllButton.setIcon(ImageFileFormat::loadFrom(BinaryData::clearicon_png, BinaryData::clearicon_pngSize));
    addAndMakeVisible(clearAllButton);

    pauseAllButton.setTooltip("Pause all tracks");
    addAndMakeVisible(pauseAllButton);

    createTracksLayoutButton();
}

GlobalControlArea::~GlobalControlArea(){
}

void GlobalControlArea::paint (juce::Graphics&){
}

void GlobalControlArea::resized(){
    auto bounds = getLocalBounds().reduced(15);
    globalLabel.setBounds(bounds.removeFromTop(15));

    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    Component trackLayoutButtonArea{};

    juce::Grid grid;
    

    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(muteAllButton), juce::GridItem(startAllButton),
        juce::GridItem(stopAllButton), juce::GridItem(clearAllButton),
        juce::GridItem(pauseAllButton), juce::GridItem(trackLayoutButtonArea)
    };

    grid.performLayout(bounds);
    
    auto trackLayoutButtonBounds = trackLayoutButtonArea.getBoundsInParent();
    tracksLayoutLeft.setBounds(trackLayoutButtonBounds.removeFromLeft(trackLayoutButtonBounds.getWidth() / 2));
    tracksLayoutRight.setBounds(trackLayoutButtonBounds);
    
}

void GlobalControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}

void GlobalControlArea::createTracksLayoutButton(){
    auto leftBase = new DrawableImage();
    leftBase->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangeleftbase_png, BinaryData::layoutchangeleftbase_pngSize));
    auto leftHover = new DrawableImage();
    leftHover->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangelefthover_png, BinaryData::layoutchangelefthover_pngSize));
    auto leftClicked = new DrawableImage();
    leftClicked->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangeleftclicked_png, BinaryData::layoutchangeleftclicked_pngSize));
    auto leftActive = new DrawableImage();
    leftActive->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangeleftactive_png, BinaryData::layoutchangeleftactive_pngSize));

    tracksLayoutLeft.setImages(leftBase, leftHover, leftClicked, nullptr, leftActive, leftHover, leftClicked, nullptr);
    tracksLayoutLeft.setTooltip("Toggles between horizontal and vertical layout of the tracks");
    tracksLayoutLeft.setToggleState(true, NotificationType::dontSendNotification);
    tracksLayoutLeft.addListener(this);
    tracksLayoutLeft.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnRight);
    addAndMakeVisible(tracksLayoutLeft);


    auto rightBase = new DrawableImage();
    rightBase->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangerightbase_png, BinaryData::layoutchangerightbase_pngSize));
    auto rightHover = new DrawableImage();
    rightHover->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangerighthover_png, BinaryData::layoutchangerighthover_pngSize));
    auto rightClicked = new DrawableImage();
    rightClicked->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangerightclicked_png, BinaryData::layoutchangerightclicked_pngSize));
    auto rightActive = new DrawableImage();
    rightActive->setImage(ImageFileFormat::loadFrom(BinaryData::layoutchangerightactive_png, BinaryData::layoutchangerightactive_pngSize));

    tracksLayoutRight.setImages(rightBase, rightHover, rightClicked, nullptr, rightActive, rightHover, rightClicked, nullptr);
    tracksLayoutRight.setTooltip("Toggles between horizontal and vertical layout of the tracks");
    tracksLayoutRight.setToggleState(false, NotificationType::dontSendNotification);
    tracksLayoutRight.addListener(this);
    tracksLayoutRight.setConnectedEdges(Button::ConnectedEdgeFlags::ConnectedOnLeft);
    addAndMakeVisible(tracksLayoutRight);
}

void GlobalControlArea::buttonClicked(Button* button){
    if (editor == nullptr ){
        return;
    }
    if (button == &muteAllButton) {
        toggleMuteAll((bool)muteAllButton.getToggleStateValue().getValue());
    }
    if (button == &clearAllButton) {
        editor->toggleClear();
    }
    if (button == &tracksLayoutLeft) {
        if (!editor->isTrackLayoutHorizontal()){
            editor->toggleLayout();
            tracksLayoutLeft.setToggleState(true, NotificationType::dontSendNotification);
            tracksLayoutRight.setToggleState(false, NotificationType::dontSendNotification);
        }

        editor->setTracksDirty();
        editor->repaint();
        editor->resized();
    }
    if (button == &tracksLayoutRight) {
        if (editor->isTrackLayoutHorizontal()) {
            editor->toggleLayout();
            tracksLayoutLeft.setToggleState(false, NotificationType::dontSendNotification);
            tracksLayoutRight.setToggleState(true, NotificationType::dontSendNotification);
        }

        editor->setTracksDirty();
        editor->repaint();
        editor->resized();
    }
}

void GlobalControlArea::toggleMuteAll(bool) {
    muteAllButton.setState(Button::buttonNormal);
}
