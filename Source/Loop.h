#pragma once
#ifndef __LOOP_GUARD
#define __LOOP_GUARD
#include "Orbish.h"
#include "Context.h"
#endif



class Loop
{
public:
    Loop(){};
    ~Loop(){};

	std::vector<Layer*> Layers;
	uint32 Index = 0;
	int CurrentPlayingIndex = 0;
	int LoopDuration = 0;
	bool LayersReady = true;
	bool FirstPlaybackBuffer = false;
	bool LastPlaybackBuffer = false;

    int CurrentTop = -1;

	double Progress = 0;

    
	void AddLayer(bool incrementTop, OrbishContext* context) {
		LayersReady = false;
		if (!context->layerQueue->empty()) {
			long start1 = 0, end1 = 0, start2 = 0, end2 = 0;
			start1 = Time::getHighResolutionTicks();
			Layer* l = nullptr;
			do {
				if (l != nullptr) {
					delete l->Buffer;
					delete  l;
				}
				l = context->layerQueue->front();
            } while (l != nullptr && l->Buffer->getNumChannels() == 0);
            l->index = int(Layers.size());
			Layers.push_back(l);
				end1 = Time::getHighResolutionTicks();
				start2 = Time::getHighResolutionTicks();
			context->layerQueue->pop();
				end2 = Time::getHighResolutionTicks();
				context->logMessage("time for Layers->add: " + String(end1 - start1));
				context->logMessage("time for pop_front: " + String(end2 - start2));
			if (incrementTop) CurrentTop = int(Layers.size()) - 1;
		}
		LayersReady = true;
	}
    Layer* activePlaybackLayer;
    Layer* activeRecordingLayer;
};


