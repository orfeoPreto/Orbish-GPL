/*
  ==============================================================================

    InfoArea.cpp
    Created: 1 Aug 2020 3:22:24pm
    Author:  lenna

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InfoArea.h"

//==============================================================================
InfoArea::InfoArea(){
    projectLabel.setFont(Font(18, Font::bold));
    addAndMakeVisible(projectLabel);

    timeSigLabel.setTooltip("Current time signature\nComes from the host");
    addAndMakeVisible(timeSigLabel);

    addAndMakeVisible(bpmLabel);

    progressLabel.setTooltip("Shows the progress of the loop while playing\n Time progress is synced to the host");
    addAndMakeVisible(progressLabel);

    addAndMakeVisible(trackNumberLabel);
    addAndMakeVisible(loopNumberLabel);
    addAndMakeVisible(layerNumberLabel);
    addAndMakeVisible(groupNumberLabel);
}

InfoArea::~InfoArea(){
}

void InfoArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (juce::Colours::blue);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    projectLabel.setColour(Label::textColourId, findColour(TextButton::ColourIds::textColourOnId));

    // draw beatWitness
    Path path;
    path.addEllipse(20, 30*7+21, 10, 10);
    beatWitness.setPath(path);
    beatWitness.setFill(Colours::orangered);
    beatWitness.setStrokeFill(Colours::orange);
    beatWitness.setAlpha(beatAlpha);
    beatWitness.setStrokeThickness(2.0f);
    addAndMakeVisible(barWitness);

    // draw barWitness
    Path path2;
    path2.addEllipse(40, 30*7+21, 10, 10);
    barWitness.setPath(path2);
    barWitness.setFill(Colours::green);
    barWitness.setStrokeFill(Colours::greenyellow);
    barWitness.setAlpha(barAlpha);
    barWitness.setStrokeThickness(2.0f);
    addAndMakeVisible(beatWitness);
}

void InfoArea::resized(){
    auto bounds = getLocalBounds();
    auto labelHeight = 30;
    bounds.removeFromLeft(10);
    bounds.removeFromTop(10);

    projectLabel.setBounds(bounds.removeFromTop(labelHeight));

    trackNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    loopNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    layerNumberLabel.setBounds(bounds.removeFromTop(labelHeight));

    groupNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    timeSigLabel.setBounds(bounds.removeFromTop(labelHeight));
    bpmLabel.setBounds(bounds.removeFromTop(labelHeight));
    
    bounds.removeFromLeft(50);
    progressLabel.setBounds(bounds.removeFromTop(labelHeight));
}

void InfoArea::setProjectName(String name){
    projectLabel.setText(name, NotificationType::dontSendNotification);
}

void InfoArea::setTimeSignature(String timeSig){
    timeSigLabel.setText(timeSig, NotificationType::dontSendNotification);
    timeSigLabel.touch();
}

String InfoArea::getTimeSignature(){
    return timeSigLabel.getText();
}

void InfoArea::setBeatsPerMinute(String bpm){
    bpmLabel.setText(bpm, NotificationType::dontSendNotification);
    bpmLabel.touch();
}

String InfoArea::getBeatsPerMinute(){
    return bpmLabel.getText();
}

void InfoArea::setProgress(String progress){
    progressLabel.setText(progress, NotificationType::dontSendNotification);
}

String InfoArea::getProgress(){
    return progressLabel.getText();
}

void InfoArea::setTrackNumber(String trackNumber){
    trackNumberLabel.setText(trackNumber, NotificationType::dontSendNotification);
}

String InfoArea::getTrackNumber(){
    return trackNumberLabel.getText();
}

void InfoArea::setLoopNumber(String loopNumber){
    loopNumberLabel.setText(loopNumber, NotificationType::dontSendNotification);
}

String InfoArea::getLoopNumber(){
    return loopNumberLabel.getText();
}

void InfoArea::setLayerNumber(String layerNumber){
    layerNumberLabel.setText(layerNumber, NotificationType::dontSendNotification);
}

String InfoArea::getLayerNumber(){
    return layerNumberLabel.getText();
}

void InfoArea::setGroupNumber(String groupNumber){
    groupNumberLabel.setText(groupNumber, NotificationType::dontSendNotification);
}

String InfoArea::getGroupNumber(){
    return groupNumberLabel.getText();
}

void InfoArea::updateBarWitness(double alpha){
    barAlpha = alpha;
}

void InfoArea::updateBeatWitness(double alpha){
    beatAlpha = alpha;
}
