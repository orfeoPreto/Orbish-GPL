/*
  ==============================================================================

    InfoArea.cpp
    Created: 1 Aug 2020 3:22:24pm
    Author:  Lennart Cockx

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
    String s = String(ProjectInfo::projectName) + " " + String(ProjectInfo::versionString) + "\nBy " + String(ProjectInfo::companyName);
    logo.setTooltip(s);
    logo.setImage(ImageFileFormat::loadFrom(BinaryData::orbishlogowebdown_png, BinaryData::orbishlogowebdown_pngSize));

  //  clickButton.setTooltip("Enable/disable click");
//    addAndMakeVisible(clickButton);
}

InfoArea::~InfoArea(){
}

void InfoArea::setOpenGLContext(std::shared_ptr<OpenGLContext> ctxt){
    openGLContext = ctxt;
    witness->setOpenGLContext(openGLContext);
    addAndMakeVisible(*witness);
    witness->start();
}

void InfoArea::setWitness(std::shared_ptr<OpenGLClickWitness> witness){
    this->witness = witness;
}


void InfoArea::paint (juce::Graphics& g){
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    projectLabel.setColour(Label::textColourId, findColour(TextButton::ColourIds::textColourOnId));
    projectLabel.touch();
    progressLabel.touch();
    trackNumberLabel.touch();
    loopNumberLabel.touch();
    layerNumberLabel.touch();
    groupNumberLabel.touch();
    timeSigLabel.touch();
    bpmLabel.touch();
}

void InfoArea::resized(){
    addAndMakeVisible(name);
    addAndMakeVisible(logo);
    auto bounds = getLocalBounds();
    auto labelHeight = 30;
    bounds.removeFromLeft(10);
    bounds.removeFromTop(80);
    projectLabel.setBounds(bounds.removeFromTop(labelHeight));
    trackNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    loopNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    layerNumberLabel.setBounds(bounds.removeFromTop(labelHeight));
    groupNumberLabel.setBounds(bounds.removeFromTop(labelHeight));

    //bounds.removeFromTop(labelHeight);

    timeSigLabel.setBounds(bounds.removeFromTop(labelHeight));
    bpmLabel.setBounds(bounds.removeFromTop(labelHeight));
    
    bounds.removeFromLeft(50);
    progressLabel.setBounds(bounds.removeFromTop(labelHeight));
    witness->setBounds(20, progressLabel.getY()-5, int(std::ceil(labelHeight)), int(std::ceil(labelHeight)));
//    clickButton.setBounds(5, witness->getY(), witness->getHeight() * .7, witness->getHeight() *.7);
    auto ratio = logo.getImage().getHeight() / 50;
    logo.setBounds(10,20, logo.getImage().getWidth()/ratio,50);
    logo.setAlpha(.8f);
}

void InfoArea::setProjectName(String name){
    projectLabel.setText(name, NotificationType::dontSendNotification);
    projectLabel.touch();
    projectLabel.repaint();
}

void InfoArea::setTimeSignature(String timeSig){
    timeSigLabel.setText(timeSig, NotificationType::dontSendNotification);
    timeSigLabel.touch();
    timeSigLabel.repaint();
}

String InfoArea::getTimeSignature(){
    return timeSigLabel.getText();
}
void InfoArea::setSubDivs(float subDivs){
    witness->setOffset(subDivs);
}

void InfoArea::setBeatsPerMinute(String bpm){
    bpmLabel.setText(bpm, NotificationType::dontSendNotification);
    bpmLabel.touch();
    bpmLabel.repaint();
}

String InfoArea::getBeatsPerMinute(){
    return bpmLabel.getText();
}

void InfoArea::setProgress(String progress){
    progressLabel.setText(progress, NotificationType::dontSendNotification);
    progressLabel.touch();
    progressLabel.repaint();
}

String InfoArea::getProgress(){
    return progressLabel.getText();
}

void InfoArea::setTrackNumber(String trackNumber){
    trackNumberLabel.setText(trackNumber, NotificationType::dontSendNotification);
    trackNumberLabel.repaint();
}

String InfoArea::getTrackNumber(){
    return trackNumberLabel.getText();
}

void InfoArea::setLoopNumber(String loopNumber){
    loopNumberLabel.setText(loopNumber, NotificationType::dontSendNotification);
    loopNumberLabel.repaint();
}

String InfoArea::getLoopNumber(){
    return loopNumberLabel.getText();
}

void InfoArea::setLayerNumber(String layerNumber){
    layerNumberLabel.setText(layerNumber, NotificationType::dontSendNotification);
    layerNumberLabel.repaint();
}

String InfoArea::getLayerNumber(){
    return layerNumberLabel.getText();
}

void InfoArea::setGroupNumber(String groupNumber){
    groupNumberLabel.setText(groupNumber, NotificationType::dontSendNotification);
    groupNumberLabel.repaint();
}

String InfoArea::getGroupNumber(){
    return groupNumberLabel.getText();
}

void InfoArea::setGroupColour(Colour c){
    groupNumberLabel.setColour(Label::textColourId,c);
}

void InfoArea::updateBarWitness(double alpha){
    barAlpha = alpha;
}

void InfoArea::updateBeatWitness(double alpha){
    beatAlpha = alpha;
}
