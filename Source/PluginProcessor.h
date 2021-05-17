/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#ifndef Processor_h
#define Processor_h

#include "JuceHeader.h"
#include "Track.h"
#include "MidiProcessor.h"
#include "TrackGroup.h"
#include "HostSynchronizer.h"



//==============================================================================
/**
*/
struct Events{
    int startRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be started
    int stopRecordingSample = -1; // sample number in buffer when activeTrack->Recording should be stopped
    int startPlayingSample = -1; // sample number in buffer when playback should be started
    int stopPlayingSample = -1; // sample number in buffer when playback should be stopped
    int startReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be started
    int stopReverseSample = -1; // sample number in buffer when activeTrack->Reverse should be stopped
    int toggleMuteSample = -1; //sample number in buffer when Mute state should be toggled
    int toggleSoloSample = -1; //sample number in buffer when Solo state should be toggled
    int startLoopChangeSample = -1;
    bool toggleSolo = false;
};

class OrbishAudioProcessor  : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
{
public:
    enum GlobalAction{
        kGlobalNone,
        kGlobalStop,
        kGlobalPlay,
        kGlobalPause,
        kGlobalResume,
        kGlobalMute,
        kGlobalReset
    };
    
    //==============================================================================
    OrbishAudioProcessor();
    ~OrbishAudioProcessor();

    //==============================================================================
    std::unique_ptr<RangedAudioParameter> createParamFromBool(AudioParameterBool* boolParam, bool defaultValue);
    std::unique_ptr<RangedAudioParameter> createParamFromInt(AudioParameterInt* intParam, int defaultValue);

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void parameterChanged(const String &parameterID, float newValue) override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    bool isEditorActive();
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    FFAU::LevelMeterSource* getInputMeterSource ();
    FFAU::LevelMeterSource* getOutputMeterSource ();
    FFAU::LevelMeterSource* getGlobalMeterSource ();

    //==============================================================================
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    void processMidi(const MidiBuffer& midi);
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    void changeTrack(int &nextTrackIndex);
    void removeTrack(int &removeTrackIndex);
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void initBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages);
    void realign();
    void renderClick();
    void smoothVolume(double& origin, double destination, int samplesToRead, AudioBuffer<float>* source, AudioBuffer<float>* target, int channel);
    
    void captureTrigger(int& startRecordingSample);
    void processMuteAllChange(bool mute);
    void processStopAllChange();
    void processStartAllChange();
    void processPauseAllChange(bool pause);
    void processResetAllChange();
    void processTrackChange(uint newTrackIndex);
    void processNewTrack();
    void processRemoveTrack(int);
    void processGroupSelect(int);
    void processAddToGroup(int);
    void processRemoveFromGroup(int);
    void processClickToggle();
    void handleRecordBlock(int start, int stop);
    void handlePlaybackBlock(int start, int stop);
    void addTrack(bool);
    void askTrackChange(int trackNumber);
	void askLoopChange(int loopNumber);
    void cleanup();
    void handleReverseEvent(int, int);
    void handleRecordingEvent(int, int);
    void handlePlaybackEvent(int, int);
    void handleMuteEvent(int);
    bool handleSoloEvent(int);
    void handleLoopChangeEvent(int);
    void handleEvents(Events&);
    void processTempoChange(int);
    TrackGroup* getTrackGroup(Track* t);
	bool loadFromValueTree(ValueTree* tree);
	bool loadTrackFromValueTree(ValueTree* trackTree, Track* track);
	bool loadLoopFromValueTree(ValueTree* loopTree, Loop* loop);
    void handleClick(std::shared_ptr<OrbishContext> context, AudioSampleBuffer*);
	void initGroups();
    
    bool clickInProcess = false;
    int numBuffersDoneForClick = 0;
    std::shared_ptr<OrbishContext> context;
    TrackGroup* CurrentGroup;
	TrackGroup* SelectedGroup;
    //std::unique_ptr<RubberBand::RubberBandStretcher> stretcher;
    void initParams();
    Track* activeTrack = nullptr;
	AudioProcessorEditor* editor;
	OwnedArray<Track, CriticalSection> tracks;
	bool guiAlive = false;
	std::vector<TrackGroup*> groups;
	private:
    FFAU::LevelMeterSource inputMeterSource;
    FFAU::LevelMeterSource outputMeterSource;
    int nextTrack = -1;
    int trackToRemove = -1;
    void startPlayback();
    void stopPlayback();
    void resumePlayback();
    void pausePlayback();
    void startReset();
    void init();
    void doMute();
    MidiProcessor* midiProcessor;
    MessageManager* messenger;
    AudioProcessorValueTreeState parameters;
    bool hostHasPlayed = false;
    GlobalAction globalAction = kGlobalNone;
	bool keepRunning = true;
    bool queuesEmpty = false;
    bool aTrackIsSoloed = false;
    double previousMixLevel = -1;
    bool changingTrack = false;
    std::unique_ptr<Synchronizer> primarySynchronizer;
    std::unique_ptr<Synchronizer> secondarySynchronizer;
    int trackHostSamples = 0;

    //===========^*==================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessor)
};

#endif
