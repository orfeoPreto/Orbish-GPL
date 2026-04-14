/*
  ==============================================================================

    ThumbnailAndGroupArea.cpp
    Created: 3 Aug 2020 2:13:15pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ThumbnailAndGroupArea.h"

//==============================================================================
ThumbnailAndGroupArea::ThumbnailAndGroupArea(){
    addAndMakeVisible(thumbnailArea);

    timeLabel.setJustificationType(juce::Justification::centredRight);
    timeLabel.setFont(juce::Font(14.0f, juce::Font::plain));
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.6f));
    timeLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(timeLabel);

    setOpaque(true);
}

ThumbnailAndGroupArea::~ThumbnailAndGroupArea(){
}

void ThumbnailAndGroupArea::paint (juce::Graphics& g){
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Waveform container: rounded corners with subtle accent border
    auto containerBounds = getLocalBounds().reduced(2).toFloat();
    auto accent = findColour(juce::TextButton::ColourIds::buttonOnColourId);
    auto innerBg = findColour(juce::TextButton::ColourIds::buttonColourId);

    g.setColour(innerBg.withAlpha(0.15f));
    g.fillRoundedRectangle(containerBounds, 10.0f);
    g.setColour(accent.withAlpha(0.08f));
    g.drawRoundedRectangle(containerBounds, 10.0f, 0.5f);
}

void ThumbnailAndGroupArea::paintOverChildren(juce::Graphics& g){
    // Draw a rounded-corner frame that masks the sharp edges of the GL thumbnail
    auto bounds = getLocalBounds().toFloat();
    auto containerBounds = bounds.reduced(2.0f);
    float radius = 10.0f;
    float frameWidth = 6.0f; // enough to cover the thumbnail edges

    auto bg = getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId);
    auto accent = findColour(juce::TextButton::ColourIds::buttonOnColourId);

    // Create a path that is the full bounds minus the rounded rect interior
    juce::Path frame;
    frame.addRectangle(bounds);
    frame.addRoundedRectangle(containerBounds.reduced(frameWidth * 0.5f), radius);
    frame.setUsingNonZeroWinding(false);

    g.setColour(bg);
    g.fillPath(frame);

    // Redraw the accent border on top
    g.setColour(accent.withAlpha(0.08f));
    g.drawRoundedRectangle(containerBounds, radius, 0.5f);
}

void ThumbnailAndGroupArea::resized(){
    auto bounds = getLocalBounds();
    thumbnailArea.setBounds(bounds.reduced(4));

    // Time label in upper-right corner
    timeLabel.setBounds(bounds.getWidth() - 200, 4, 192, 22);
}

void ThumbnailAndGroupArea::updateTimeDisplay(const juce::String& positionTime, const juce::String& totalTime) {
    auto text = positionTime + " / " + totalTime;
    if (timeLabel.getText() != text)
        timeLabel.setText(text, juce::NotificationType::dontSendNotification);
}
