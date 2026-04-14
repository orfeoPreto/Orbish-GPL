/*
  ==============================================================================

    NavigationControlArea.cpp
    Created: 4 Aug 2020 4:24:03pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "NavigationControlArea.h"

//==============================================================================
NavigationControlArea::NavigationControlArea(){
    loopLabel.setText("Loop", NotificationType::dontSendNotification);
    addAndMakeVisible(loopLabel);
    
    previousLoopButton.setTooltip("Go to previous loop on active track or group");
    addAndMakeVisible(previousLoopButton);

    activeLoopLabel.setText("1", NotificationType::dontSendNotification);
    activeLoopLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(activeLoopLabel);

    nextLoopButton.setTooltip("Go to next loop on active track or group");
    addAndMakeVisible(nextLoopButton);

    newLoopButton.setTooltip("Create new loop on active track");
    addAndMakeVisible(newLoopButton);

    removeLoopButton.setTooltip("Remove latest loop from active track");
    addAndMakeVisible(removeLoopButton);
    


    trackLabel.setText("Track", NotificationType::dontSendNotification);
    addAndMakeVisible(trackLabel);

    previousTrackButton.setTooltip("Go to previous track");


    addAndMakeVisible(previousTrackButton);

    activeTrackLabel.setText("1", NotificationType::dontSendNotification);
    activeTrackLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(activeTrackLabel);

    nextTrackButton.setTooltip("Go to next track");
    addAndMakeVisible(nextTrackButton);

    newTrackButton.setTooltip("Create new track");
    addAndMakeVisible(newTrackButton);

    removeTrackButton.setTooltip("Remove latest track");
    addAndMakeVisible(removeTrackButton);
}

NavigationControlArea::~NavigationControlArea(){
}

void NavigationControlArea::paint (juce::Graphics& g){

    auto bounds = getLocalBounds();
    auto loopButtonArea = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(5);
    auto trackButtonArea = bounds.reduced(5);
//    projectLabel.setColour(Label::textColourId, findColour(TextButton::ColourIds::textColourOnId));
    trackLabel.touch();
    loopLabel.touch();
    activeLoopLabel.touch();
    activeTrackLabel.touch();
    g.setColour(findColour(juce::TextButton::ColourIds::buttonOnColourId).withAlpha(0.06f));
    g.drawRoundedRectangle(loopButtonArea.toFloat(), 7.0f, 0.5f);
    g.drawRoundedRectangle(trackButtonArea.toFloat(), 7.0f, 0.5f);
}

void NavigationControlArea::resized(){
    auto bounds = getLocalBounds();

    auto layoutSection = [](juce::Rectangle<int> area, exu::Label& label, exu::Label& activeLabel,
                            CustomButton& prevBtn, CustomButton& nextBtn,
                            CustomButton& addBtn, CustomButton& removeBtn) {
        auto section = area.reduced(8, 4);
        label.setBounds(section.removeFromTop(15));
        section.removeFromTop(2);
        auto row1 = section.removeFromTop(section.getHeight() / 2);
        auto row2 = section;
        auto bw = row1.getWidth() / 3;
        prevBtn.setBounds(row1.removeFromLeft(bw));
        activeLabel.setBounds(row1.removeFromLeft(bw));
        nextBtn.setBounds(row1);
        addBtn.setBounds(row2.removeFromLeft(row2.getWidth() / 2));
        removeBtn.setBounds(row2);
    };

    // Loop navigation
    auto loopArea = bounds.removeFromLeft(bounds.getWidth() / 2);
    layoutSection(loopArea, loopLabel, activeLoopLabel,
                  previousLoopButton, nextLoopButton, newLoopButton, removeLoopButton);

    // Track navigation
    trackLabel.setText("Track", NotificationType::sendNotification);
    layoutSection(bounds, trackLabel, activeTrackLabel,
                  previousTrackButton, nextTrackButton, newTrackButton, removeTrackButton);
}

void NavigationControlArea::setActiveLoop(String loopNumber){
    activeLoopLabel.setText(loopNumber, NotificationType::dontSendNotification);
}

void NavigationControlArea::setActiveTrack(String trackNumber){
    activeTrackLabel.setText(trackNumber, NotificationType::dontSendNotification);
}
