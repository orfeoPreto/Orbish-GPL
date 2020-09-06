//
//  Track.hpp
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
#endif


enum TrackGroupCommands {
	Play,
	Stop,
	Mute,
	LoopChange
};


class Track :public AudioProcessorValueTreeState::Listener {
public:


	Track(uint, bool, AudioProcessorValueTreeState& params, OrbishContext*& c, bool& gui);

	~Track();

	OwnedArray<Loop> loops;
	Loop* ActiveLoop;
	std::vector<Layer*>* Layers;
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
	float PreviousInputLevel;
	float PreviousOutputLevel;
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
	int RealignOffset = 0;
	bool Realigned = false;
	int BeginFadeOffset = 0;
	int EndFadeOffset = 0;
	bool loopToBeExtended = false;
	bool doBounce = false;
    int nextLoop = -1;
    bool loopChangeArmed = false;
	typedef void (Track::* RunAfterBlock) ();
	std::vector<RunAfterBlock> RunAfters;
	int* CurrentTop = nullptr;
    bool& guiAlive;
	double* Progress;

	void AddLayer(bool incrementTop);

	void RemoveTopLayer();
    
    void UpdateLoopVisualizer();

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
    
    void RemoveLoopAfter();
    
    void setActive(bool newValue);
    
    bool isActive();
    
	inline bool IsPlaying()
	{
		// while Recording and overwriting content without blending, do not play
		return (Playing || (LastPlaybackBuffer)) && !(Recording && ((getRecordMode() == kRecOverWrite || getRecordMode() == kRecAppend) && CurrentRecordingIndex > *LoopDuration));
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
	void processBounceChange();
    void processLoopChange(int);
    void processNewLoop();
    void processRemoveLoop();

    
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
    SnapMode getSnapMode();
    RecordMode getRecordMode();
	ValueTree* state;
    std::shared_ptr<FileLogger> logger;
    void setState(ValueTree*);
private:
	AudioProcessorValueTreeState& params;
    bool active = false;
    OrbishContext* &context;
};
