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
    for(int i=0;i<progress.size();++i){
        auto loop = new LoopComponent(*progress[i], i);
        String str = "Loop " + String(i+1) + " in track " + String(index + 1);
        loop->setTooltip(str);
        Loops.add(loop);
    }


    addAndMakeVisible(witness);
    trackNameLabel.setText(this->getName(), NotificationType::sendNotification);
    trackNameLabel.setFont(Font(10.0f));
    trackNameLabel.setColour(Label::textWhenEditingColourId, Colours::white);
    trackNameLabel.setEditable(false,true,false);

    trackNameLabel.addListener(this);
    addAndMakeVisible(trackNameLabel);
    trackNumberLabel.setFont(Font(10.0f));
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
}
int TrackComponent::getActiveLoop(){
    return activeLoop;
}

void TrackComponent::removeLoop(){
    Loops.removeLast();
}
void TrackComponent::addLoop(double& p){
    Loops.add(new LoopComponent(p, Loops.size()));
}

void TrackComponent::setActive(bool active){
    this->active = active;
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

void TrackComponent::resized(){
    repaint();
    auto newBounds = getBoundsInParent();
    setBounds(newBounds);
    trackNumberLabel.setBounds(margin , margin, buttonSize + 5, buttonSize);

    highlighter.setBounds(margin*2 + buttonSize , 1.5f*margin, buttonSize * .8f , buttonSize *.8f);
	groupLabel.setBounds(margin + 1.7f * (margin + buttonSize), margin, buttonSize * 2, buttonSize);
    trackNameLabel.setBounds(2.5f * (margin + buttonSize), margin, 100, buttonSize);
    int startHorizontalLoop = trackNameLabel.getX() + trackNameLabel.getWidth() - buttonSize;
    auto r = getLocalBounds();
    if(!horizontalLayout){
        for(auto l: Loops){
            auto i = l->getIndex();
            if((i+2)*loopHeight >= getHeight()){
                break;
            }
            l->setBounds(r.getX() + margin, r.getY() +margin+loopHeight+ (i * (loopHeight)), r.getWidth() - 2*margin, loopHeight-margin) ;
        }
        if(tempProgressBar != nullptr){
            tempProgressBar->setVisible(false);
        }
    } else {
        for (auto l : Loops) {
            startHorizontalLoop+=margin+buttonSize;
            if(activeLoop == l->getIndex()){
                delete tempProgressBar;
                tempProgressBar = new ProgressBar(l->getProgress());
                tempProgressBar->setPercentageDisplay(false);
                l->ProgressBar::copyAllExplicitColoursTo(*tempProgressBar);
                addAndMakeVisible(tempProgressBar);
                tempProgressBar->setBounds(r.getX() + margin, r.getY()+margin+loopHeight, r.getWidth() - 2*margin, loopHeight-2*margin) ;
            }
            l->setBounds(startHorizontalLoop, margin, buttonSize, buttonSize);
        }
    }
}

//void TrackComponent::mouseDrag() {
//    
//}

void TrackComponent::paint(Graphics& g){
    auto r = getLocalBounds();
    if(active){
            Path pth{};
            pth.addRectangle(r.withSizeKeepingCentre(r.getWidth()+10, r.getHeight()+10));

    }else{
        g.setColour(Colour(0x0F262C36));
        g.fillRect(r);
    }
    
    auto ed = trackNameLabel.getCurrentTextEditor();
    if(nullptr != ed){
        ed->setColour(TextEditor::highlightedTextColourId, Colours::white);
        ed->setColour(TextEditor::highlightColourId, Colours::blue);
    }
    
    highlighter.setColour(Blinker::offColourId, Colour(0x6FFFFFFF));

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
        highlighter.state = Blinker::kNeutral;
    }
    if (highlighter.state != Blinker::kNeutral){
        addAndMakeVisible(highlighter);
    }
    
	groupLabel.setText(" " + Group , NotificationType::dontSendNotification);
	groupLabel.setColour(Label::textColourId, GroupColour);
    witness.setStrokeThickness (2.0f);
    
  for (auto l: Loops) {
        Colour bgColour, fgColour;
        if(active){
            if(activeLoop == l->getIndex()){
                bgColour = Colour(0xFFC5C5C5);
                fgColour = Colour(0xFF0000FF);
            }else{
                bgColour = Colour(0xFF8F8F8F);
                fgColour = Colour(0xFF002366);
            }
        }else{
            if(activeLoop == l->getIndex()){
                bgColour = Colour(0xFFB3B3B3);
                fgColour = Colour(0xFF00008B);
            }else{
                bgColour = Colour(0xFFA9A9A9);
                fgColour = Colour(0xFF001133);
            }
        }
        l->setColour(ProgressBar::ColourIds::backgroundColourId, bgColour);
        l->setColour(ProgressBar::ColourIds::foregroundColourId, fgColour);
        addAndMakeVisible(l);
    }

}

void TrackComponent::parentSizeChanged() {
    resized();
}

void TrackComponent::buttonClicked (Button* button) {
    
}

void TrackComponent::sliderValueChanged (Slider* slider) {
    
}

void TrackComponent::changeListenerCallback (ChangeBroadcaster* source) {
    
}

