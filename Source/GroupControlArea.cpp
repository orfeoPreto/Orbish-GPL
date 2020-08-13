/*
  ==============================================================================

    GroupControlArea.cpp
    Created: 6 Aug 2020 12:38:51pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GroupControlArea.h"
#include "PluginEditor.h"

//==============================================================================
GroupControlArea::GroupControlArea(){
    groupLabel.setText("Group", juce::NotificationType::dontSendNotification);
    addAndMakeVisible(groupLabel);

    groupCombo.setTooltip("Select a group, then add or remove tracks. \nAll tracks in the same group will act simultaneously for certain commands");
    groupCombo.setSelectedId(1);
    addAndMakeVisible(groupCombo);

    addToGroupButton.setTooltip("Add the active track to the selected group");
    addAndMakeVisible(addToGroupButton);

    removeFromGroupButton.setTooltip("Remove the active track from the selected group");
    addAndMakeVisible(removeFromGroupButton);
}

GroupControlArea::~GroupControlArea(){
}

void GroupControlArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle(getLocalBounds().reduced(5).toFloat(), 4.0f, 1.0f);
}

void GroupControlArea::resized(){
    auto bounds = getLocalBounds().reduced(15);

    groupLabel.setBounds(bounds.removeFromTop(15));
    groupCombo.setBounds(bounds.removeFromTop(bounds.getHeight() / 2).reduced(10));
    addToGroupButton.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    removeFromGroupButton.setBounds(bounds);
}

void GroupControlArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
}

void GroupControlArea::buttonClicked(Button* button){
    if (editor == nullptr){
        return;
    }
    if (button == &addToGroupButton) {
        editor->makeTracks();
    }
    if (button == &removeFromGroupButton) {
        editor->makeTracks();
    }
}
