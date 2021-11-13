#pragma once
#ifndef __LOOP_GUARD
#define __LOOP_GUARD
#include "Orbish.h"
#include "Context.h"
#endif



class Loop
{
public:
    Loop ();
    Loop(int index);
    ~Loop();

	std::shared_ptr<std::vector<std::shared_ptr<Layer> > > Layers;
	uint32 Index = 0;
	int CurrentPlayingIndex = 0;
	int LoopDuration = 0;
	bool LayersReady = true;
	bool FirstPlaybackBuffer = false;
	bool LastPlaybackBuffer = false;

    int CurrentTop = -1;

	std::atomic<float> Progress;

    
    void AddLayer(bool incrementTop, std::shared_ptr<OrbishContext> context);
    void setActivePlaybackLayer(std::shared_ptr<Layer> l);

    void updateFlattenedVisualizationBuffer();
    void clearFlattenedVisualizationBuffer();
    std::shared_ptr<Layer> activePlaybackLayer;
    std::shared_ptr<Layer> activeRecordingLayer;
    float flattenedVisualizationBuffer[BUFFER_READ_SIZE];

};


