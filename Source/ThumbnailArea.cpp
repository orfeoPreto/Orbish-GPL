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
