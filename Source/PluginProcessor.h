/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#ifndef Processor_h
#define Processor_h

#include "../JuceLibraryCode/JuceHeader.h"
#include "Track.h"
#include "MidiProcessor.h"
#include "TrackGroup.hpp"



//==============================================================================
/**
*/

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
    void captureTrigger(int startRecordingSample);
    float samplesPerMinute = 0;
    float secondsPerSample = 0;
    inline double quartersToSamples(double position)
    {
        return position * samplesPerMinute / context->bpm;
    }
    
    inline double beatsToSamples(double position)
    {
        return position * samplesPerMinute / context->bpm * context->timeSigBottom * .25f;
    }
    
    inline double samplesToQuarters(double samples)
    {
        return samples * context->bpm / (samplesPerMinute);
    }
    inline double samplesToBeats(double samples)
    {
        return samples * context->bpm / samplesPerMinute * (context->timeSigBottom * .25);
    }
    
    inline double differenceFromClosestBeatInSamples(int position){
        auto diff = position % context->samplesPerBeat;
        if (diff > context->samplesPerBeat/2){
            diff = diff - context->samplesPerBeat;
        }
        return double(diff);
    }


    
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
    void handleRecordBlock(int start, int stop);
    void handlePlaybackBlock(int start, int stop);
    void addTrack(bool);
    void askTrackChange(int trackNumber);
	void askLoopChange(int loopNumber);
    void cleanup();
    TrackGroup* getTrackGroup(Track* t);
	bool loadFromValueTree(ValueTree* tree);
	bool loadTrackFromValueTree(ValueTree* trackTree, Track* track);
	bool loadLoopFromValueTree(ValueTree* loopTree, Loop* loop);
    void handleClick(OrbishContext* context, int start, int stop);
	void initGroups();
    OrbishContext* context;
    TrackGroup* CurrentGroup;
	TrackGroup* SelectedGroup;
    void initParams();
    Track* activeTrack;
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
    void doMute();
    MidiProcessor* midiProcessor;
    MessageManager* messenger;
    AudioProcessorValueTreeState parameters;
    bool hostHasPlayed = false;
    int realignOffset = 0;
    bool realigned = false;
    GlobalAction globalAction = kGlobalNone;
	bool keepRunning = true;
    bool aTrackIsSoloed = false;
    //===========^*==================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessor)
};

#endif
