//
//  TrackComponent.cpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 01/05/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "TrackComponent.h"

TrackComponent::TrackComponent():Component()
{
    
}


TrackComponent::TrackComponent(int index, std::vector<double*> progress, bool& layout, Track* track) :Component("Track " + String(index + 1)),
                                            horizontalLayout(layout),
                                            buttonSize(15),
                                            margin(3)
{
    this->index = index;
    setAudioTrack(track);
	loopHeight = margin + buttonSize;
    for(int i=0;i<progress.size() && (nullptr != progress[i]);++i){
        auto loop = new LoopComponent(*progress[i], i);
        String str = "Loop " + String(i+1) + " in track " + String(index + 1);
        loop->setTooltip(str);
        Loops.add(loop);
    }

    tempProgressBar = new ProgressBar(Loops.getFirst()->getProgress());
    tempProgressBar->setPercentageDisplay(false);
    Loops.getFirst()->ProgressBar::copyAllExplicitColoursTo(*tempProgressBar);
    addChildComponent(tempProgressBar);

    addAndMakeVisible(witness);

    trackNameLabel.setText(this->getName(), NotificationType::sendNotification);
    trackNameLabel.setFont(Font(14.0f));
    trackNameLabel.setColour(Label::textWhenEditingColourId, Colours::white);
    trackNameLabel.setEditable(false,true,false);
    trackNameLabel.addListener(this);
    addAndMakeVisible(trackNameLabel);

    trackNumberLabel.setFont(Font(14.0f));
    trackNumberLabel.setText(String(index+1), NotificationType::dontSendNotification);
    trackNumberLabel.addListener(this);
    trackNumberLabel.setTooltip("index of the track");
    trackNameLabel.setTooltip("Name of the track. \nDouble-click to edit");
    highlighter.setTooltip("Highlights the current state of the track:\n\tGreen: Playing\n\tRed: Recording\n\tBlue: Muted\n\tOrange: Soloed");
    addAndMakeVisible(trackNumberLabel);

	groupLabel.setFont(Font(10.0f));
	groupLabel.addListener(this);
    groupLabel.setTooltip("The group this track belongs to");

	addAndMakeVisible(groupLabel);
    setPaintingIsUnclipped(true);
}

TrackComponent::~TrackComponent(){
    
}

void TrackComponent::setActiveLoop(int loopIdx){
    activeLoop =  loopIdx;
    if(Loops.size() < loopIdx+1)return;
    delete tempProgressBar;
    tempProgressBar = new ProgressBar(Loops[loopIdx]->getProgress());
    tempProgressBar->setPercentageDisplay(false);
    addChildComponent(tempProgressBar);

    updateLoopColours();
    resized();
}
int TrackComponent::getActiveLoop(){
    return activeLoop;
}

void TrackComponent::removeLoop(){
    Loops.removeLast();
    resized();
}
void TrackComponent::addLoop(double& p){
    LoopComponent* newLoop = new LoopComponent(p, Loops.size());
    String str = "Loop " + String(newLoop->getIndex() + 1) + " in track " +
    String(index + 1);
    newLoop->setTooltip(str);
    Loops.add(newLoop);
    resized();
}

void TrackComponent::setActive(bool isActive){
    this->active = isActive;
    updateLoopColours();
}

bool TrackComponent::isActive(){
    return active;
}

void TrackComponent::setRecording(bool isRecording){
    this->recording = isRecording;
}
bool TrackComponent::isRecording(){
    return recording;
}
void TrackComponent::setPlaying(bool isPlaying){
    this->playing = isPlaying;
}
bool TrackComponent::isPlaying(){
    return playing;
}
void TrackComponent::setMuted(bool isMuted){
    this->muted = isMuted;
}
bool TrackComponent::isMuted(){
    return muted;
}
void TrackComponent::setSoloed(bool isSoloed){
    this->soloed = isSoloed;
}
bool TrackComponent::isSoloed(){
    return soloed;
}

void TrackComponent::setRecordingArmed(bool isRecordingArmed){
    this->recordingArmed = isRecordingArmed;
}
bool TrackComponent::isRecordingArmed(){
    return recordingArmed;
}
void TrackComponent::setPlayArmed(bool isPlayArmed){
    this->playArmed = isPlayArmed;
}
bool TrackComponent::isPlayArmed(){
    return playArmed;
}
void TrackComponent::setMutedArmed(bool isMuteArmed){
    this->muteArmed = isMuteArmed;
}
bool TrackComponent::isMutedArmed(){
    return muteArmed;
}
void TrackComponent::setSoloArmed(bool isSoloArmed){
    this->soloArmed = isSoloArmed;
}
bool TrackComponent::isSoloArmed(){
    return soloArmed;
}

void TrackComponent::labelTextChanged (Label* label){
    setName(label->getText());
}


int TrackComponent::getIndex(){
    return index;
}

Track* TrackComponent::getAudioTrack(){
    return audioTrack;
}

void TrackComponent::setAudioTrack(Track* t){
    audioTrack = t;
}

void TrackComponent::setGroup(String group, Colour colour){
    Group = group;
    GroupColour = colour;
}

String TrackComponent::getGroup(){
    return Group;
}

void TrackComponent::updateLoopColours(){
    for (auto l : Loops) {
        if (active) {
            if (activeLoop == l->getIndex()) {
                l->setColour(juce::ProgressBar::foregroundColourId, juce::Colour(0xfffddc11));
                l->setColour(juce::ProgressBar::backgroundColourId, juce::Colour(0xff707070));
                l->ProgressBar::copyAllExplicitColoursTo(*tempProgressBar);
            }
            else {
                l->setColour(juce::ProgressBar::foregroundColourId, juce::Colour(0xfff2e499));
                l->setColour(juce::ProgressBar::backgroundColourId, juce::Colour(0xff42403a));
            }
        }
        else {
            if (activeLoop == l->getIndex()) {
                l->setColour(juce::ProgressBar::foregroundColourId, juce::Colour(0xffc1a402));
                l->setColour(juce::ProgressBar::backgroundColourId, juce::Colour(0xff42403a));
                l->ProgressBar::copyAllExplicitColoursTo(*tempProgressBar);
            }
            else {

                l->setColour(juce::ProgressBar::foregroundColourId, juce::Colour(0xffb49e53));
                l->setColour(juce::ProgressBar::backgroundColourId, juce::Colour(0xff42403a));
            }
        }
    }
    repaint();
}

void TrackComponent::resized(){
    repaint();
    trackNumberLabel.setBounds(margin , margin, buttonSize + 5, buttonSize);

    highlighter.setBounds(margin*2 + buttonSize , int(1.5f*margin), int(buttonSize * .8f) , int(buttonSize *.8f));
	groupLabel.setBounds(int(margin + 3.0f) * (margin + buttonSize), margin, buttonSize * 2, buttonSize);
    trackNameLabel.setBounds(int(2.5f * (margin + buttonSize)), margin, 100, buttonSize);
    int startHorizontalLoop = trackNameLabel.getX() + trackNameLabel.getWidth() - buttonSize;

    auto bounds = getLocalBounds().reduced(2*margin);

    if(!horizontalLayout){
        for(auto loop: Loops){
            int index = loop->getIndex();
            loop->setBounds(bounds.getX(), bounds.getY()+ loopHeight+(index * (loopHeight)), bounds.getWidth(), loopHeight-margin) ;
        }
        if(tempProgressBar != nullptr){
            tempProgressBar->setVisible(false);
        }
    } else {
        for (auto l : Loops) {
            tempProgressBar->setBounds(bounds.getX(), bounds.getY() + loopHeight, bounds.getWidth(), loopHeight);
            l->setBounds(startHorizontalLoop, margin, 55, buttonSize);
            startHorizontalLoop += margin + 55;
        }
        if (tempProgressBar != nullptr) {
            tempProgressBar->setVisible(true);
        }
    }
}

void TrackComponent::paint(Graphics& g){
    auto r = getLocalBounds();

    g.setColour(findColour(backgroundColourId));
    g.fillRect(r);
    if(active){
        g.setColour(findColour(outlineColourId));
        g.drawRoundedRectangle(r.toFloat(), 2.5f, 1.5f);
    }
    
    auto ed = trackNameLabel.getCurrentTextEditor();
    if(nullptr != ed){
        ed->setColour(TextEditor::highlightedTextColourId, Colours::white);
        ed->setColour(TextEditor::highlightColourId, Colours::blue);
    }

    if(isSoloed() || isSoloArmed()){
        if(isSoloed()){
            highlighter.setColour(Blinker::onColourId, Colour(0xFFFFBB33));
            if(isPlayArmed() && !isPlaying()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFF39C70D));
                highlighter.state = Blinker:: kArmed;
            }else if(isRecordingArmed() && !isRecording()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFFFF1919));
                highlighter.state = Blinker:: kArmed;
            }else if(isMutedArmed() && !isMuted()){
                    highlighter.setColour(Blinker::offColourId, Colour(0xFF0077B3));
                    highlighter.state = Blinker:: kArmed;
            }else{
                highlighter.state = Blinker:: kHighlighted;
            }
        }else{
            if(isRecording() || isPlaying() || isMuted()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFFFFBB33));
            }else{
                highlighter.setColour(Blinker::onColourId, Colour(0xFFFFBB33));
            }
            highlighter.state = Blinker:: kArmed;
        }
    }else if(isRecording() || isRecordingArmed()){
        if(isRecording()){
            highlighter.setColour(Blinker::onColourId, Colour(0xFFFF1919));
            if(isPlayArmed() && !isPlaying()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFF39C70D));
                highlighter.state = Blinker:: kArmed;
            }else if(isMuted() && !isMuted()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFF0077B3));
                highlighter.state = Blinker:: kArmed;
            }else{
                highlighter.state = Blinker:: kHighlighted;
            }
        }else{
            if(isPlaying() || isMuted()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFFFF1919));
            }else{
                highlighter.setColour(Blinker::onColourId, Colour(0xFFFF1919));
            }
            highlighter.state = Blinker:: kArmed;
        }
    }else if(isMuted() || isMutedArmed()){
        if(isMuted()){
            highlighter.setColour(Blinker::onColourId, Colour(0xFF0077B3));
            if(isPlayArmed() && !isPlaying()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFF39C70D));
                highlighter.state = Blinker:: kArmed;
            }else{
                highlighter.state = Blinker:: kHighlighted;
            }
        }else{
            if(isPlaying()){
                highlighter.setColour(Blinker::offColourId, Colour(0xFF0077B3));
            }else{
                highlighter.setColour(Blinker::onColourId, Colour(0xFF0077B3));
            }
            highlighter.state = Blinker:: kArmed;
        }
    }else if(isPlaying() || isPlayArmed()){
        highlighter.setColour(Blinker::onColourId, Colour(0xFF39C70D));
        if(isPlaying()){
            highlighter.state = Blinker:: kHighlighted;
        }else{
            highlighter.state = Blinker:: kArmed;
        }
    }else {
        highlighter.setColour(Blinker::offColourId, Colour(0x6FFFFFFF));
        highlighter.state = Blinker::kNeutral;
        highlighter.setEnabled(false);
    }
    addAndMakeVisible(highlighter);
    
	groupLabel.setText(" " + Group , NotificationType::dontSendNotification);
	groupLabel.setColour(Label::textColourId, GroupColour);
    witness.setStrokeThickness (2.0f);
    
    for (auto l: Loops) {
      addAndMakeVisible(l);
    }

}

void TrackComponent::parentSizeChanged() {
    resized();
}

void TrackComponent::buttonClicked (Button* ) {
    
}

void TrackComponent::sliderValueChanged (Slider* ) {
    
}

void TrackComponent::changeListenerCallback (ChangeBroadcaster* ) {
    
}

