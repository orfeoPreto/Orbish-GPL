//
//  TimeStretchRubberBand.h
//  Orbish
//
//  Created by Duke Quarcoo on 24/09/2025.
//  Copyright © 2025 exu. All rights reserved.
//

#ifndef TimeStretchRubberBand_h
#define TimeStretchRubberBand_h


#endif /* TimeStretchRubberBand_h */
#pragma once
#include "rubberband/rubberband/RubberBandStretcher.h"
using RubberBand::RubberBandStretcher;

class TimeStretchRubberBand {
public:
    TimeStretchRubberBand() : stretcher(nullptr), sampleRate(44100), channels(2) {}
    ~TimeStretchRubberBand() { reset(); }

    void prepare(int sr, int ch) {
        sampleRate = sr; channels = ch;
        reset();
        if (sr <= 0 || ch <= 0) return;
        int options = RubberBandStretcher::OptionProcessRealTime
                    | RubberBandStretcher::OptionTransientsCrisp
                    | RubberBandStretcher::OptionPitchHighConsistency
                    | RubberBandStretcher::OptionThreadingAuto;
        stretcher = std::make_unique<RubberBandStretcher>(sampleRate, channels, options, 1.0, 1.0);
        // Set small window/hop for low-latency:
        stretcher->setTimeRatio(1.0);
        stretcher->setPitchScale(1.0);
        // Request minimal internal latency where possible
        stretcher->setMaxProcessSize(2048); // keep relatively small
    }

    void reset() {
        stretcher.reset();
    }

    // set tempo/time ratio: newTempo / originalTempo
    void setTimeRatio(double ratio) {
        if (stretcher) stretcher->setTimeRatio(ratio);
    }

    // set pitch as linear ratio (1.0 = no change)
    void setPitchRatio(double pitchRatio) {
        if (stretcher) stretcher->setPitchScale(pitchRatio);
    }

    // process interleaved channel buffers: input pointers length samplesPerChannel
    // rubberband accepts deinterleaved per-channel pointers as const float* const*
    void process(const float* const* inPtrs, int frames, bool finalBlock = false) {
        if (!stretcher) return;
        stretcher->process(inPtrs, frames, finalBlock);
    }

    // retrieve up to frames samples into outPtrs; returns how many frames were retrieved
    int retrieve(float** outPtrs, int maxFrames) {
        if (!stretcher) return 0;
        int got = stretcher->retrieve(outPtrs, maxFrames);
        return got;
    }
    private:
    std::unique_ptr<RubberBandStretcher> stretcher;
        int sampleRate;
        int channels;
    };
