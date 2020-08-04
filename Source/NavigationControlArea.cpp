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
    loopLabel.setText("Loops: ", NotificationType::dontSendNotification);
    addAndMakeVisible(loopLabel);

    previousLoopButton.setTooltip("Go to previous loop on active track or group");
    addAndMakeVisible(previousLoopButton);

    activeLoopLabel.setText("1", NotificationType::dontSendNotification);
    addAndMakeVisible(activeLoopLabel);

    nextLoopButton.setTooltip("Go to next loop on active track or group");
    addAndMakeVisible(nextLoopButton);

    newLoopButton.setTooltip("Create new loop on active track");
    addAndMakeVisible(newLoopButton);

    removeLoopButton.setTooltip("Remove latest loop from active track");
    addAndMakeVisible(removeLoopButton);
    
    
    


}

NavigationControlArea::~NavigationControlArea(){
}

void NavigationControlArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

void NavigationControlArea::resized(){
    auto bounds = getLocalBounds();

    auto loopButtonArea = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(5);
    auto loopNavArea = loopButtonArea.removeFromLeft(loopButtonArea.getWidth() * 3 / 5);

    loopLabel.setBounds(loopNavArea.removeFromTop(15));
    auto buttonWidth = loopNavArea.getWidth() / 3;
    previousLoopButton.setBounds(loopNavArea.removeFromLeft(buttonWidth));
    activeLoopLabel.setBounds(loopNavArea.removeFromLeft(buttonWidth));
    nextLoopButton.setBounds(loopNavArea);

    newLoopButton.setBounds(loopButtonArea.removeFromTop(loopButtonArea.getWidth()/2));
    removeLoopButton.setBounds(loopButtonArea);

}

void NavigationControlArea::setActiveLoop(String loopNumber){
    activeLoopLabel.setText(loopNumber, NotificationType::dontSendNotification);
}
