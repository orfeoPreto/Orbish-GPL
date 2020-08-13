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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    auto bounds = getLocalBounds();
    auto loopButtonArea = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(5);
    auto trackButtonArea = bounds.reduced(5);

    g.drawRect(loopButtonArea);
    g.drawRect(trackButtonArea);
}

void NavigationControlArea::resized(){
    auto bounds = getLocalBounds();

    // Loop navigation
    auto loopButtonArea = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(10);
    auto loopNavArea = loopButtonArea.removeFromLeft(loopButtonArea.getWidth() * 3 / 5);
    
    loopLabel.setBounds(loopNavArea.removeFromTop(15));
    auto buttonWidth = loopNavArea.getWidth() / 3;
    previousLoopButton.setBounds(loopNavArea.removeFromLeft(buttonWidth));
    activeLoopLabel.setBounds(loopNavArea.removeFromLeft(buttonWidth));
    nextLoopButton.setBounds(loopNavArea);
    newLoopButton.setBounds(loopButtonArea.removeFromTop(loopButtonArea.getHeight()/2));
    removeLoopButton.setBounds(loopButtonArea);

    // Track navigation
    auto trackButtonArea = bounds.reduced(10);
    auto trackNavArea = trackButtonArea.removeFromLeft(trackButtonArea.getWidth() * 3 / 5);

    trackLabel.setBounds(trackNavArea.removeFromTop(15));
    buttonWidth = trackNavArea.getWidth() / 3;
    previousTrackButton.setBounds(trackNavArea.removeFromLeft(buttonWidth));
    activeTrackLabel.setBounds(trackNavArea.removeFromLeft(buttonWidth));
    nextTrackButton.setBounds(trackNavArea);
    newTrackButton.setBounds(trackButtonArea.removeFromTop(trackButtonArea.getHeight() / 2));
    removeTrackButton.setBounds(trackButtonArea);

}

void NavigationControlArea::setActiveLoop(String loopNumber){
    activeLoopLabel.setText(loopNumber, NotificationType::dontSendNotification);
}

void NavigationControlArea::setActiveTrack(String trackNumber){
    activeTrackLabel.setText(trackNumber, NotificationType::dontSendNotification);
}
