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
    addAndMakeVisible(muteAllButton);

    startAllButton.setTooltip("Start playing on all tracks");
    addAndMakeVisible(startAllButton);

    stopAllButton.setTooltip("Stop playing on all tracks");
    addAndMakeVisible(stopAllButton);

    clearAllButton.setTooltip("Clear all tracks");
    addAndMakeVisible(clearAllButton);

    pauseAllButton.setTooltip("Pause all tracks");
    addAndMakeVisible(pauseAllButton);

    createTracksLayoutButton();
    addAndMakeVisible(tracksLayoutButton);
}

GlobalControlArea::~GlobalControlArea(){
}

void GlobalControlArea::paint (juce::Graphics& g){

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void GlobalControlArea::resized(){
    auto bounds = getLocalBounds().reduced(10);
    globalLabel.setBounds(bounds.removeFromTop(15));

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.templateRows = { Track(Fr(1)), Track(Fr(1)), Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(muteAllButton), juce::GridItem(startAllButton),
        juce::GridItem(stopAllButton), juce::GridItem(clearAllButton),
        juce::GridItem(pauseAllButton), juce::GridItem(tracksLayoutButton)
    };

    grid.performLayout(bounds);
}

void GlobalControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}

void GlobalControlArea::createTracksLayoutButton()
{
    auto horizontalOutline = new DrawableComposite();
    for (int i = 0; i < 3; ++i) {
        Path pth;
        auto* line = new DrawablePath();
        pth.addLineSegment(Line<float>{2, float(i) * 8, 30, float(i) * 8}, 4);
        line->setPath(pth);
        horizontalOutline->addChildComponent(line);
    }

    auto verticalOutline = new DrawableComposite();
    for (int i = 0; i < 6; ++i) {
        Path pth;
        auto* block = new DrawablePath();
        pth.addRectangle(Rectangle<int>{i % 3 * 10, i / 3 * 13, 5, 7});
        block->setPath(pth);
        verticalOutline->addChildComponent(block);
    }

    tracksLayoutButton.setColour(DrawableButton::backgroundOnColourId, Colour(0x40FFFFFF));
    tracksLayoutButton.setColour(DrawableButton::backgroundColourId, Colour(0x40FFFFFF));

    tracksLayoutButton.setImages(horizontalOutline, horizontalOutline, horizontalOutline, nullptr, verticalOutline, verticalOutline, verticalOutline, nullptr);
    tracksLayoutButton.setTooltip("Toggles between horizontal and vertical layout of the tracks");
    tracksLayoutButton.setToggleState(true, NotificationType::dontSendNotification);
    tracksLayoutButton.setClickingTogglesState(true);
    tracksLayoutButton.addListener(this);
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
    if (button == &tracksLayoutButton) {
        editor->toggleLayout();
        editor->setTracksDirty();
        editor->repaint();
        editor->resized();
    }
}

void GlobalControlArea::toggleMuteAll(bool mute) {
    muteAllButton.setState(Button::buttonNormal);
}
