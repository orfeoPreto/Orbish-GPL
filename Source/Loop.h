#pragma once
#ifndef __LOOP_GUARD
#define __LOOP_GUARD
#include "Orbish.h"
#include "Context.h"
#endif



class Loop
{
public:
    Loop(int index){
        Index = index;
        Layers = std::make_shared<std::vector<std::shared_ptr<Layer> > >();
        Layers->reserve(100);
    };
    ~Loop(){};

	std::shared_ptr<std::vector<std::shared_ptr<Layer> > > Layers;
	uint32 Index = 0;
	int CurrentPlayingIndex = 0;
	int LoopDuration = 0;
	bool LayersReady = true;
	bool FirstPlaybackBuffer = false;
	bool LastPlaybackBuffer = false;

    int CurrentTop = -1;

	double Progress = 0;

    
	void AddLayer(bool incrementTop, std::shared_ptr<OrbishContext> context) {
		LayersReady = false;
		if (!context->xchange->layerQueue->empty()) {
			long start1 = 0, end1 = 0, start2 = 0, end2 = 0;
			start1 = Time::getHighResolutionTicks();
			std::shared_ptr<Layer> l = nullptr;
            l = context->xchange->layerQueue->front();
            l->index = int(Layers->size());
			Layers->push_back(l);
				end1 = Time::getHighResolutionTicks();
				start2 = Time::getHighResolutionTicks();
			context->xchange->layerQueue->pop();
				end2 = Time::getHighResolutionTicks();
				context->logMessage("time for Layers->add: " + String(end1 - start1));
				context->logMessage("time for pop_front: " + String(end2 - start2));
			if (incrementTop) CurrentTop = int(Layers->size()) - 1;
		}
		LayersReady = true;
	}
    std::shared_ptr<Layer> activePlaybackLayer;
    std::shared_ptr<Layer> activeRecordingLayer;
};


