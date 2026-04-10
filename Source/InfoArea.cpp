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
    setOpaque(true);


    clickButton.setTooltip("Enable/disable click");
    // Pendulum metronome icon - asymmetric mid-swing feel
    // Drawn in a 24×24 viewBox, scaled to button size
    {
        const int sz = 24;
        // ON icon: pendulum arm + weight + base
        {
            Image img(Image::ARGB, sz, sz, true);
            Graphics g(img);
            g.setColour(Colours::white);
            // Diagonal arm from (6,20) to (15,4)
            g.drawLine(6.0f, 20.0f, 15.0f, 4.0f, 1.8f);
            // Pendulum weight at top of arm
            g.fillEllipse(15.0f - 1.8f, 4.0f - 1.8f, 3.6f, 3.6f);
            // Base line
            g.drawLine(4.0f, 20.0f, 18.0f, 20.0f, 1.5f);
            clickButton.setIcon(img);
        }
        // OFF icon: same pendulum + red mute slash from (3,3) to (21,21)
        {
            Image img(Image::ARGB, sz, sz, true);
            Graphics g(img);
            g.setColour(Colours::white);
            g.drawLine(6.0f, 20.0f, 15.0f, 4.0f, 1.8f);
            g.fillEllipse(15.0f - 1.8f, 4.0f - 1.8f, 3.6f, 3.6f);
            g.drawLine(4.0f, 20.0f, 18.0f, 20.0f, 1.5f);
            // Mute slash in record-red
            g.setColour(Colour(0xffe84545));
            g.drawLine(3.0f, 3.0f, 21.0f, 21.0f, 1.5f);
            clickButton.setIconOff(img);
        }
    }
    addAndMakeVisible(clickButton);

    clickLevelSlider.setSliderStyle(Slider::LinearHorizontal);
    clickLevelSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    clickLevelSlider.setTooltip("Click volume");
    // Click slider colors are driven by the LookAndFeel accent (Slider::thumbColourId)
    // and will be set when the theme is applied
    addAndMakeVisible(clickLevelSlider);
}

InfoArea::~InfoArea(){
}

void InfoArea::setOpenGLContext(std::shared_ptr<OpenGLContext> ctxt){
    openGLContext = ctxt;
    witness->setOpenGLContext(openGLContext, false);
//    witness->setTopLevelComponent(getTopLevelComponent());
    witness->setLookAndFeel(&getLookAndFeel());
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
    auto cbSize = witness->getHeight();
    clickButton.setBounds(witness->getX(), witness->getBottom(), cbSize, cbSize);
    clickLevelSlider.setBounds(clickButton.getRight() + 4, clickButton.getY(), 80, cbSize);
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
void InfoArea::setSubDivs(std::atomic<float> &subDivs){
    witness->setOffset(std::ref(subDivs));
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
