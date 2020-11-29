#pragma once
#ifndef __CTX_GUARD
#define __CTX_GUARD
#include "Orbish.h"
//#include "Loop.h"
#endif

#define DEBUG_MODE 1
//#include <thread>
#include <mutex>
#include "Observer.h"
#include <deque>
#include <boost/lockfree/spsc_queue.hpp>
#include "DataExchange.h"
//#include "TrackGroup.hpp"

typedef unsigned int uint;

struct Layer {
	AudioBuffer<float>* Buffer{};
    int Checkpoint = -1;
	bool dirty = false;
    int index = 0;
    bool FirstLayerBuffer = false;
    bool LastLayerBuffer = false;
};

struct OrbishContext {
	OrbishContext() {
		buffer = nullptr;
        inputBuffer = nullptr;
		audioInputsCount = 0;
		previousMix = 0;
		delayCompensation = 0;
		audioOutputsCount = 0;
		auxAudioInputsCount = 0;
		auxAudioOutputsCount = 0;
		maxBlockSize = 0;
		sampleRate = 44100;
		allocatedLength = 0;
		fadeTime = 0; // 1ms fade time
		defaultLoopLength = 120; //120 seconds max activeTrack.Recording
		timeRatio = 0;
		samplesPerBlock = 0;
		trackCount = 0;
		loopCount = 0;
		fadeInc = 0;
		samplesPerBeat = 0;
		bpm = 0;
		timeSigBottom = 1;
		timeSigTop = 1;
		info = new AudioPlayHead::CurrentPositionInfo();
	}
	~OrbishContext() {

	}

	uint32    audioInputsCount;
	float mix = 0;
	float previousMix = 0;
	int delayCompensation = 0;
	uint32    audioOutputsCount;
	uint32    auxAudioInputsCount;
	uint32    auxAudioOutputsCount;
	int     maxBlockSize;
	int  sampleRate = 44100;
	String  userDocumentsPath;
	String  scriptFilePath;
	const double triggerThreshold = .005;
	int allocatedLength;
	int fadeTime; // 1ms fade time
	std::thread allocatorThread;
	int defaultLoopLength; // 60 seconds max activeTrack.Recording
	float timeRatio;
	AudioBuffer<float>* buffer = nullptr;
    AudioBuffer<float>* inputBuffer = nullptr;
    std::unique_ptr<AudioBuffer<float>> clickBuffer;
    std::unique_ptr<AudioBuffer<float>> barStartClickBuffer;
	int samplesPerBlock;
	Observer* observer;
	int trackCount;
	int loopCount;
	Array<double*> progress{};
	std::mutex mtx;
	int maxUndoHistory = -1;
	AudioPlayHead::CurrentPositionInfo* info;
	bool init = false;
	float feedback;
	float fadeInc;
	int samplesPerBeat;
	int bpm;
	int timeSigBottom;
	int timeSigTop;
	int hey;
	boost::lockfree::spsc_queue<Layer*, boost::lockfree::capacity<3> > * layerQueue;
	DataExchange* xchange;
    bool loggingActive = false;
    int clickStart=-1, clickStop=0;
    int64 timestamp = 0;
    float maxDelta = 0.1;
    bool skipAlign = false;

    inline double quartersToSamples(double position)
    {
        return position * samplesPerMinute / bpm;
    }
    
    inline double beatsToSamples(double position)
    {
        return position * samplesPerMinute / bpm * timeSigBottom * .25f;
    }
    
    inline double samplesToQuarters(double samples)
    {
        return samples * bpm / (samplesPerMinute);
    }
    inline double samplesToBeats(double samples)
    {
        return samples * bpm / samplesPerMinute * (timeSigBottom * .25);
    }
    
    inline double differenceFromClosestBeatInSamples(int position){
        auto diff = position % samplesPerBeat;
        if (diff > samplesPerBeat/2){
            diff = diff - samplesPerBeat;
        }
        return double(diff);
    }
    
    float samplesPerMinute = 0;
    float secondsPerSample = 0;
    void lockForStateUpdate(bool lock) {
        if (lock) {
            mtx.lock();
        }
        else {
            mtx.unlock();
        }
    }
    std::shared_ptr<FileLogger> logger;
    
    void logMessage(juce::String msg) {
        if (loggingActive){
            std::string* message;
            if(xchange->logWriteMessageQueue->read_available()){
                xchange->logWriteMessageQueue->pop(message);
                message->replace(0, msg.length(), msg.toStdString());
                if (loggingActive && xchange->logReadMessageQueue->write_available()){
                    xchange->logReadMessageQueue->push(message);
                }
            }
        }
    }
    
    void flushLogs(){
        while (xchange->logWriteMessageQueue->write_available() > 0) {
            auto s = new std::string();
            s->reserve(200);
            xchange->logWriteMessageQueue->push(s);
        }
        while(xchange->logReadMessageQueue->read_available()){
            std::string* message = 0;
            xchange->logReadMessageQueue->pop(message);
            if(message != 0){
                if(loggingActive){
                    logger->logMessage(*message);
                }
#if DEBUG_MODE
                    DBG(*message);
#endif
            }
        }
    }
};
