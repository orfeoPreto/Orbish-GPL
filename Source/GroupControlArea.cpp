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

    // Hidden combo still used for APVTS attachment
    groupCombo.setTooltip("Select a group, then add or remove tracks. \nAll tracks in the same group will act simultaneously for certain commands");
    groupCombo.setVisible(false);
    groupCombo.addListener(this);
    addChildComponent(groupCombo);

    // Navigation buttons
    previousGroupButton.setTooltip("Previous group");
    previousGroupButton.addListener(this);
    addAndMakeVisible(previousGroupButton);

    activeGroupLabel.setText("1", juce::NotificationType::dontSendNotification);
    activeGroupLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(activeGroupLabel);

    nextGroupButton.setTooltip("Next group");
    nextGroupButton.addListener(this);
    addAndMakeVisible(nextGroupButton);

    addToGroupButton.setTooltip("Add the active track to the selected group");
    addToGroupButton.addListener(this);
    addToGroupButton.setIcon(ImageFileFormat::loadFrom(BinaryData::groupicon_png, BinaryData::groupicon_pngSize));
    addAndMakeVisible(addToGroupButton);

    removeFromGroupButton.setTooltip("Remove the active track from the selected group");
    removeFromGroupButton.addListener(this);
    removeFromGroupButton.setIcon(ImageFileFormat::loadFrom(BinaryData::ungroupicon_png, BinaryData::ungroupicon_pngSize));
    addAndMakeVisible(removeFromGroupButton);
}

GroupControlArea::~GroupControlArea(){
}

void GroupControlArea::paint (juce::Graphics& g){
    // GroupControlArea is currently hidden - group controls in nav panel
}

void GroupControlArea::resized(){
    auto bounds = getLocalBounds().reduced(15);

    groupLabel.setBounds(bounds.removeFromTop(15));
    groupCombo.setBounds(bounds.removeFromTop(60).reduced(10));
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
    if (button == &previousGroupButton) {
        auto current = groupCombo.getSelectedId();
        if (current > 1)
            groupCombo.setSelectedId(current - 1);
    }
    if (button == &nextGroupButton) {
        auto current = groupCombo.getSelectedId();
        if (current < groupCombo.getNumItems())
            groupCombo.setSelectedId(current + 1);
    }
    if (button == &addToGroupButton) {
        editor->makeTracks();
    }
    if (button == &removeFromGroupButton) {
        editor->makeTracks();
    }
    // Update displayed group name
    activeGroupLabel.setText(groupCombo.getText(), juce::NotificationType::dontSendNotification);
}

void GroupControlArea::comboBoxChanged(ComboBox* combo) {
    if (combo == &groupCombo)
        activeGroupLabel.setText(groupCombo.getText(), juce::NotificationType::dontSendNotification);
}
