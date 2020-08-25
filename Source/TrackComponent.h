//
//
//  TrackComponent.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 01/05/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef TrackComponent_h
#define TrackComponent_h

#include <stdio.h>
#include "JuceHeader.h"
#include "LoopComponent.hpp"
#include "Blinker.hpp"
#include "../exu/Label.hpp"
#include "Track.h"

class TrackComponent:
        public Component,
        public ChangeListener,
        public Button::Listener,
        public Slider::Listener,
        public Label::Listener{
public:
    
    TrackComponent();
    TrackComponent(int index, std::vector<double*> progress, bool& layout, Track* track);
    
    ~TrackComponent();
    enum ColourIds
    {
        backgroundColourId                  = 0x2000101,
        activeBackgroundColourId            = 0x2000102,
        outlineColourId                     = 0x2000103
    };
    
    static void test(){
        return;
    }
    void buttonClicked (Button* button) override;
    void sliderValueChanged (Slider* slider) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;
	void resized() override;
    void paint(Graphics& g) override;
    void parentSizeChanged() override;
    void setActive(bool active);
    bool isActive();
    void setRecording(bool isRecording);
    bool isRecording();
    void setPlaying(bool isPlaying);
    bool isPlaying();
    void setMuted(bool isMuted);
    bool isMuted();
    void setSoloed(bool isSoloed);
    bool isSoloed();
    void setRecordingArmed(bool isRecordingArmed);
    bool isRecordingArmed();
    void setPlayArmed(bool isPlayArmed);
    bool isPlayArmed();
    void setMutedArmed(bool isMuteArmed);
    bool isMutedArmed();
    void setSoloArmed(bool isSoloArmed);
    bool isSoloArmed();
    int getIndex();
    void labelTextChanged (Label* label) override;
    void setActiveLoop(int);
    int getActiveLoop();
    void removeLoop();
    void addLoop(double&);
    bool horizontalLayout = false;
    bool armed = false;
    OwnedArray<LoopComponent> Loops;
	int loopHeight;
    Track* getAudioTrack();
    void setAudioTrack(Track*);
    void setGroup(String group, Colour colour);
    String getGroup();

private:
    void updateLoopColours();
    String Group;
    Colour GroupColour;

    Blinker highlighter {};
    DrawablePath witness {};
    int buttonSize;
    Track* audioTrack;
    int margin;
    bool active = false;
    bool muted = false;
    bool recording = false;
    bool soloed = false;
    bool recordingArmed = false;
    bool muteArmed = false;
    bool soloArmed = false;
    bool playArmed = false;
    bool playing = false;
    int index;
    exu::Label trackNumberLabel;
    exu::Label trackNameLabel;
	exu::Label groupLabel;
    int activeLoop =0;
    ProgressBar* tempProgressBar = nullptr;
};
#endif
