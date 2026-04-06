//
//  Track.h
//  Orbish
//
//  Created by Duke Quarcoo on 23/03/2019.
//  Copyright © 2019 EXU. All rights reserved.
//
#pragma once
#ifndef __TRACK_GUARD
#define __TRACK_GUARD
#include "Orbish.h"
#include "Loop.h"
#include "InternalSynchronizer.h"
#include "TimeStretchRubberBand.h"


//forward declaration
class Realignment;


enum TrackGroupCommands {
	Play,
	Stop,
	Mute,
	LoopChange
};


class Track :public AudioProcessorValueTreeState::Listener, public Synchronizer{
public:


	Track(uint, bool, AudioProcessorValueTreeState& params, std::shared_ptr<OrbishContext>& c, bool& gui);

	~Track() override;
    int getNextSynchronizationPoint(SnapMode) override;
    
	OwnedArray<Loop> loops;
	Loop* ActiveLoop;
	std::shared_ptr<std::vector<std::shared_ptr<Layer> > > Layers;
	uint32 Index = 0;
	bool Recording = false;
	int* CurrentPlayingIndex = nullptr;
	int CurrentRecordingIndex = 0;
	int* LoopDuration;
	bool Triggered = false;
	bool Reverse = false;
	bool Playing = false;
	bool Muted = false;
    bool Soloed = false;
	double PreviousInputLevel = -1;
	double PreviousOutputLevel = -1;
	bool LayersReady = true;
	bool FirstRecordingBuffer = false;
	bool LastRecordingBuffer = false;
	bool FirstPlaybackBuffer = false;
	bool LastPlaybackBuffer = false;
	bool FirstMuteBuffer = false;
	bool LastMuteBuffer = false;
    bool FirstSoloBuffer = false;
    bool LastSoloBuffer = false;
	bool WasPlaying = false;
    bool WasRecording = false;
	int BeginFadeOffset = 0;
	int EndFadeOffset = 0;
	bool loopToBeExtended = false;
	bool doBounce = false;
    int nextLoop = 0;
    bool loopChangeArmed = false;
	typedef void (Track::* RunAfterBlock) ();
	std::vector<RunAfterBlock> RunAfters;
	int* CurrentTop = nullptr;
    bool& guiAlive;
	std::atomic<float>* Progress;
    Realignment* realignment;
    
	// Safe layer access: returns nullptr if index is out of bounds
	inline std::shared_ptr<Layer> safeLayer(int idx) const {
		if (!Layers || idx < 0 || idx >= (int)Layers->size()) return nullptr;
		return (*Layers)[idx];
	}
	// Safe CurrentTop value: returns -1 if invalid
	inline int safeCurrentTop() const {
		if (!CurrentTop || !Layers || Layers->empty()) return -1;
		int ct = *CurrentTop;
		if (ct < 0 || ct >= (int)Layers->size()) return -1;
		return ct;
	}

	std::shared_ptr<Layer> AddLayer(bool incrementTop);

	void RemoveTopLayer();
    
    void UpdateLoopVisualizer();
    
    void updateVisualizationBuffers();
  
    void UpdateLoopVisualizer(Layer*);

    void RefreshLoopVisualizer();
    
    int getAdjustedLoopPosition(int, int);

    void AddLoop();
    
    void ChangeLoop(int);
    
    void RemoveLoop();
        
    void RegisterLoop(int);

	void RemoveAllLayers();

	int BounceHistory(int startCheckPoint, int endCheckPoint);

	void BounceAllHistory();
    
    void StopRecordingBefore();
    
    void StartRecordingBefore();
    
    void StopPlaybackBefore();
    
    void PausePlaybackBefore();
    
    void StartPlaybackBefore();
    
	void StartRecordingAfter();

    void StopRecordingAfter();
    
    void StartMuteAfter();
    
    void StartMuteBefore();
    
    void StopMuteAfter();
    
    void StopMuteBefore();
    
    void StartSoloAfter();
    
    void StartSoloBefore();
    
    void StopSoloAfter();
    
    void StopSoloBefore();
	
    void StartResetBefore();
    
    void StartResetAfter();
    
	void StartPlaybackAfter();
    
	void StopPlaybackAfter();
	
	void PausePlaybackAfter();
    
    void ChangeLoopBefore(int);
        
    void ChangeLoopAfter();
    
    void RemoveLoopBefore();
    
    void SetPreviousBefore();

    void SetPreviousAfter();

    void SetNextBefore();

    void SetNextAfter();
    
    void RemoveLoopAfter();
    
    void setActive(bool newValue);
    
    bool isActive();
    
	inline bool IsPlaying()
	{
		// while Recording and overwriting content without blending, do not play
		return (Playing || (LastPlaybackBuffer))
            && !(Recording &&
                 ((getRecordMode() == kRecOverWrite || getRecordMode() == kRecAppend)
                               && CurrentRecordingIndex > *LoopDuration));
	}

	void StartReverse();

	void StopReverse();
    
    void parameterChanged(const String &parameterID, float newValue) override;
    
    void processRecordingChange();
    void processPlayChange();
    void playStateChanged();
    void processStopChange();
    void processReverseChange();
    void processMuteChange() ;
    void processSoloChange() ;
    void processPreviousChange();
    void processResetChange();
    void processNextChange();
    void processTriggerModeChange();
	void processRecModeChange();
    void processNextRecModeChange();
    void processSnapModeChange();
    void processNextSnapModeChange();
    void processIncFixedChange();
	void processBounceChange();
    void processLoopChange(int);
    void processNextLoop();
    void processPreviousLoop();
    void processNewLoop();
    void processRemoveLoop();
    void updateCachedVar(String);

    std::shared_ptr<Layer> getActivePlaybackLayer();
    void setActivePlaybackLayer(std::shared_ptr<Layer>);
    std::shared_ptr<Layer> getActiveRecordingLayer();
    void setActiveRecordingLayer(std::shared_ptr<Layer>);
    void setMuteArmed(bool newValue);
    void setSoloArmed(bool newValue);
    void setStopArmed(bool newValue);
    void setInputLevel(float newValue);
    void setOutputLevel(float newValue);
    void setReverseArmed(bool newValue);
    void setPlayArmed(bool newValue);
    void setMonitoring(bool newValue);
    void setRecordingArmed(bool newValue);
    void setAutoTrigger(bool newValue);
    void setRecMode(int newValue);
    void setSnapMode(int newValue);
    void setFixedSize(int newValue);
    bool isMuteArmed();
    bool isSoloArmed();
    bool isStopArmed();
    float getInputLevel();
    float getOutputLevel();
    bool isReverseArmed();
    bool isPlayArmed();
    bool isMonitoring();
    bool isRecordingArmed();
    bool isAutoTrigger();
    int fixedSize = 1;
    SnapMode getSnapMode();
    RecordMode getRecordMode();
	ValueTree* state;
    std::shared_ptr<FileLogger> logger;
    void setState(ValueTree*);
    std::atomic<bool> refresh;
    
    std::unique_ptr<TimeStretchRubberBand> timeStretcher;
    float perTrackPitchRatio = 1.0f; // default 1.0 (no pitch change)
    double originalTempo = 120.0; // initialize track's recording tempo if available


    
private:
	AudioProcessorValueTreeState& params;
    bool active = false;
    std::shared_ptr<OrbishContext> &context;
    bool firstPass = true;
    std::unique_ptr<Synchronizer> primarySynchronizer;
    std::unique_ptr<Synchronizer> secondarySynchronizer;
    int getLimit();
    bool muteArmed, soloArmed, stopArmed, reverseArmed, recordingArmed, playArmed, monitoring, trigger;
    float input, output;
    int snap, recMode;
};


#endif
