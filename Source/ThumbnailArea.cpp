/*
  ==============================================================================

    ThumbnailArea.cpp
    Created: 6 Aug 2020 12:38:43pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ThumbnailArea.h"
#include "PluginEditor.h"

//==============================================================================
ThumbnailArea::ThumbnailArea(){
    inputDisplay.setColours(Colours::darkgrey, Colours::indianred);
    startTimer(20);
}

ThumbnailArea::~ThumbnailArea(){
}

void ThumbnailArea::paint (juce::Graphics& g){
    return;
    g.fillAll (Colour(0x00ffffffff));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle(getLocalBounds().reduced(5).toFloat(), 4.0f, 1.0f);

    auto bounds = getLocalBounds().reduced(5);

    inputDisplay.setBounds(bounds);
    if (editor == nullptr ){
        return;
    }
    auto thumbnail = editor->getThumbnailInstance();

    if (fileLoaded) {
        auto processor = editor->getProcessor();
        
        Path pth{};
        pth.addRectangle(bounds);

        g.setColour(findColour(TextButton::ColourIds::textColourOnId));
        auto audioLength(thumbnail->getTotalLength());                                      // [12]
      //  thumbnail->drawChannels(g, bounds, 0.0, audioLength, 1.0f);
        thumbnail->start();
        g.setColour(Colours::white);
        playHead.setBounds(std::max(int(playHeadPosition) - 2, bounds.getX()), bounds.getY(), 2, bounds.getHeight());
        g.setOpacity(.4f);
        if (playHead.getX() < 0)playHead.setX(0);
        g.fillRect(playHead);
        auto c1 = Colour(0x00FFFFFF);
        auto c2 = Colour(0x8FFFFF00);
        double tmpDenom = processor->context->samplesToBeats(float(*processor->activeTrack->LoopDuration));
        int denominator = int(std::ceil(tmpDenom));
        denominator = int(denominator / (processor->context->timeSigBottom * .25));
        denominator = int(denominator / processor->context->timeSigTop) * int(60 / processor->context->info->bpm);
        denominator = jmin(jmax(denominator * 2, 8), 40);
        if (processor->activeTrack->Playing) {
            if (editor->getReverseState() == On) {
                float tailWidth =
                    float(std::min(bounds.getWidth() / denominator, bounds.getWidth() - (playHead.getX() - bounds.getX())));
                auto tail = Rectangle<float>(float(playHead.getX()), float(bounds.getY()), tailWidth, float(bounds.getHeight()));
                g.setGradientFill(ColourGradient::horizontal(c2, c1, tail));
                g.fillRect(tail);
            }
            else {
                float tailWidth = float(std::min(bounds.getWidth() / denominator, playHead.getX() - bounds.getX()));
                auto tail = Rectangle<float>(float(playHead.getX() - tailWidth), float(bounds.getY()), tailWidth, float(bounds.getHeight()));
                g.setGradientFill(ColourGradient::horizontal(c1, c2, tail));
                g.fillRect(tail);
            }
        }
    }
    else {
        g.setColour(Colours::white);
        Path pth{};
        pth.addRectangle(bounds.withSizeKeepingCentre(bounds.getWidth() + 10, bounds.getHeight() + 10));

        g.setColour(findColour(Label::textColourId));
   //     g.drawFittedText("No Loop", bounds, Justification::centred, 1);
    }
}

void ThumbnailArea::resized(){
    auto thumbnail = editor->getThumbnailInstance();
    thumbnail->setBounds(getParentComponent()->getBoundsInParent().getX()
                         , getParentComponent()->getParentComponent()->getBoundsInParent().getY()
                         ,getWidth()
                         ,getHeight());
}

void ThumbnailArea::setFileLoaded(bool loaded){
    fileLoaded = loaded;
}

void ThumbnailArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
    inputDisplay.setSamplesPerBlock(editor->getProcessor()->context->maxBlockSize);
    inputDisplay.setBufferSize(editor->getProcessor()->context->samplesPerBlock);
    thumbnail = editor->getThumbnailInstance();
    addAndMakeVisible(thumbnail.get());
}

void ThumbnailArea::updatePlayHead(int position){
    if (editor == nullptr) {
        return;
    }
    auto thumbnail = editor->getThumbnailInstance();
    auto bounds = getLocalBounds();

    if (editor->getReverseState() == On) {
        if (thumbnail->getTotalLength() > 0) {
            float audioPosition = jmax(.0f, float(thumbnail->getTotalLength() - position));
            playHeadPosition = ((audioPosition / thumbnail->getTotalLength()) * bounds.getWidth() + bounds.getX());
        }
        else {
            playHeadPosition = float(thumbnail->getTotalLength() - 1);
        }
    }
    else {
        if (thumbnail->getTotalLength() > 0) {
            float audioPosition = float(position);
            playHeadPosition = ((audioPosition / thumbnail->getTotalLength()) * bounds.getWidth() + bounds.getX());
        }
        else {
            playHeadPosition = 0;
        }
    }
    playHeadPosition = jmax(playHeadPosition, .0f);
    repaint();
}

void ThumbnailArea::timerCallback(){
  //  repaint();
}
