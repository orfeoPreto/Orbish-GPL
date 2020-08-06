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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    auto bounds = getLocalBounds();

    inputDisplay.setBounds(bounds);
    if (editor == nullptr ){
        return;
    }

    if (fileLoaded) {
        auto thumbnail = editor->getThumbnailInstance();
        auto processor = editor->getProcessor();
        
        Path pth{};
        pth.addRectangle(bounds);

        g.setColour(findColour(TextButton::ColourIds::textColourOnId));
        auto audioLength(thumbnail->getTotalLength());                                      // [12]
        thumbnail->drawChannels(g, bounds, 0.0, audioLength, 1.0f);
        g.setColour(Colours::white);
        playHead.setBounds(std::max(playHeadPosition - 2.0f, float(bounds.getX())), float(bounds.getY()), 2.0f, float(bounds.getHeight()));
        g.setOpacity(0.4);
        if (playHead.getX() < 0)playHead.setX(0);
        g.fillRect(playHead);
        auto c1 = Colour(0x00FFFFFF);
        auto c2 = Colour(0x8FFFFF00);
        double tmpDenom = processor->samplesToBeats(float(*processor->activeTrack->LoopDuration));
        int denominator = std::ceil(tmpDenom);
        denominator = denominator / (processor->context->timeSigBottom * .25);
        denominator = denominator / processor->context->timeSigTop * (60 / processor->context->info->bpm);
        denominator = jmin(jmax(denominator * 2, 8), 40);
        if (processor->activeTrack->Playing) {
            if (editor->getReverseState() == On) {
                float tailWidth =
                    std::min(bounds.getWidth() / denominator, bounds.getWidth() - (playHead.getX() - bounds.getX()));
                auto tail = Rectangle<float>(playHead.getX(), bounds.getY(), tailWidth, bounds.getHeight());
                g.setGradientFill(ColourGradient::horizontal(c2, c1, tail));
                g.fillRect(tail);
            }
            else {
                float tailWidth = std::min(bounds.getWidth() / denominator, playHead.getX() - bounds.getX());
                auto tail = Rectangle<float>(playHead.getX() - tailWidth, bounds.getY(), tailWidth, bounds.getHeight());
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
        g.drawFittedText("No Loop", bounds, Justification::centred, 1.0f);
    }
}

void ThumbnailArea::resized(){
}

void ThumbnailArea::setFileLoaded(bool loaded){
    fileLoaded = loaded;
}

void ThumbnailArea::setEditor(OrbishAudioProcessorEditor* pluginEditor){
    editor = pluginEditor;
    inputDisplay.setSamplesPerBlock(editor->getProcessor()->context->maxBlockSize);
    inputDisplay.setBufferSize(editor->getProcessor()->context->samplesPerBlock);
}

void ThumbnailArea::updatePlayHead(int position, bool reverse){
    auto thumbnail = editor->getThumbnailInstance();
    auto bounds = getLocalBounds();

    if (editor->getReverseState() == On) {
        if (thumbnail->getTotalLength() > 0) {
            float audioPosition = jmax(.0f, float(thumbnail->getNumSamplesFinished() - position));
            playHeadPosition = ((audioPosition / thumbnail->getNumSamplesFinished()) * bounds.getWidth() + bounds.getX());
        }
        else {
            playHeadPosition = thumbnail->getNumSamplesFinished() - 1;
        }
    }
    else {
        if (thumbnail->getTotalLength() > 0) {
            float audioPosition = position;
            playHeadPosition = ((audioPosition / thumbnail->getNumSamplesFinished()) * bounds.getWidth() + bounds.getX());
        }
        else {
            playHeadPosition = 0;
        }
    }
    playHeadPosition = jmax(playHeadPosition, .0f);
}

void ThumbnailArea::timerCallback(){
    repaint();
}
