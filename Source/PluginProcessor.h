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
#include "Realignment.h"
#include "TrackEventHandler.h"
#include "ProcessorState.h"
#include "AllocatorThread.h"
#include "ClickGenerator.h"
#include "MidiMapping.h"

#ifdef _MSC_VER
  #define ORBISH_FORCEINLINE __forceinline
#else
  #define ORBISH_FORCEINLINE inline __attribute__((always_inline))
#endif

class OrbishAudioProcessor  : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
                            , public TrackEventHandler

{
public:

    
    //==============================================================================
    OrbishAudioProcessor();
    ~OrbishAudioProcessor();

    //==============================================================================
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
    ORBISH_FORCEINLINE void smoothVolume(double& origin, double destination, int samplesToRead, AudioBuffer<float>* source, AudioBuffer<float>* target, int channel);
    
    void captureTrigger(int& startRecordingSample) override;
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
    void printBuffer(AudioBuffer<float> *buffer, String name);
    void processClickToggle();
    void handleRecordBlock(int start, int stop);
    void handlePlaybackBlock(int start, int stop);
    void addTrack(bool);
    void askTrackChange(int trackNumber);
	void askLoopChange(int loopNumber);
    void cleanup();
    void handleReverseEvent(int, int) override;
    void handleRecordingEvent(int, int) override;
    void handlePlaybackEvent(int, int) override;
    void handleMuteEvent(int) override;
    bool handleSoloEvent(int) override;
    void handleLoopChangeEvent(int) override;
    void handleEvents(Events&);
    void processTempoChange(double);
    TrackGroup* getTrackGroup(Track* t);

    // MIDI mapping system
    void executeMidiAction(MidiAction action, uint8_t value);
    void executeTempoUp();
    void executeTempoDown();
    void executePitchUp();
    void executePitchDown();
    MidiMappingTable midiMappings;
    MidiLearnState midiLearnState;

	void initGroups();
    ClickGenerator clickGen;
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
    TrackChangeState trackChange;
    MixState mixState;
    SyncState syncState;
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
    AllocatorThread allocator;
    std::atomic<bool> refreshAll;
    std::atomic<Track*> trackToAdd;

    //===========^*==================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrbishAudioProcessor)
};

#endif
