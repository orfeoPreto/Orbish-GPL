//
//  TimeStretchRubberBand.h
//  Orbish
//
//  Created by Duke Quarcoo on 24/09/2025.
//  Copyright © 2025 exu. All rights reserved.
//

#pragma once
#include "rubberband/rubberband/RubberBandStretcher.h"
#include <vector>
#include <cmath>
using RubberBand::RubberBandStretcher;

class TimeStretchRubberBand {
public:
    TimeStretchRubberBand() : stretcher(nullptr), sampleRate(44100), channels(2) {}
    ~TimeStretchRubberBand() { reset(); }

    void prepare(int sr, int ch, int maxBlockSize = 2048) {
        sampleRate = sr; channels = ch;
        reset();
        if (sr <= 0 || ch <= 0) return;
        int options = RubberBandStretcher::OptionProcessRealTime
                    | RubberBandStretcher::OptionEngineFiner
                    | RubberBandStretcher::OptionPitchHighConsistency
                    | RubberBandStretcher::OptionWindowShort
                    | RubberBandStretcher::OptionFormantPreserved
                    | RubberBandStretcher::OptionChannelsTogether
                    | RubberBandStretcher::OptionThreadingNever;
        stretcher = std::make_unique<RubberBandStretcher>(sampleRate, channels, options, 1.0, 1.0);
        stretcher->setTimeRatio(1.0);
        stretcher->setPitchScale(1.0);
        stretcher->setMaxProcessSize(maxBlockSize);
    }

    void reset() {
        stretcher.reset();
    }

    // Clear internal buffers while retaining current ratios.
    void resetState() {
        if (stretcher) stretcher->reset();
    }

    // Pre-feed silence to satisfy RubberBand's preferred start pad.
    // Must be called from a non-RT thread (allocates internally).
    void primeWithSilence() {
        if (!stretcher) return;
        size_t pad = stretcher->getPreferredStartPad();
        if (pad == 0) return;
        std::vector<std::vector<float>> silenceBufs(channels, std::vector<float>(pad, 0.0f));
        std::vector<const float*> ptrs(channels);
        for (int c = 0; c < channels; ++c)
            ptrs[c] = silenceBufs[c].data();
        stretcher->process(ptrs.data(), pad, false);
    }

    void setTimeRatio(double ratio) {
        if (stretcher) stretcher->setTimeRatio(ratio);
    }

    void setPitchRatio(double pitchRatio) {
        if (stretcher) stretcher->setPitchScale(pitchRatio);
    }

    // Returns true when either ratio differs from unity
    bool isActive() const {
        if (!stretcher) return false;
        double tr = stretcher->getTimeRatio();
        double ps = stretcher->getPitchScale();
        return (std::abs(tr - 1.0) > 0.001 || std::abs(ps - 1.0) > 0.001);
    }

    double getTimeRatio() const {
        if (!stretcher) return 1.0;
        return stretcher->getTimeRatio();
    }

    double getPitchScale() const {
        if (!stretcher) return 1.0;
        return stretcher->getPitchScale();
    }

    size_t getStartDelay() const {
        if (!stretcher) return 0;
        return stretcher->getStartDelay();
    }

    int available() const {
        if (!stretcher) return 0;
        return stretcher->available();
    }

    // Feed deinterleaved per-channel audio into the stretcher
    void process(const float* const* inPtrs, int frames, bool finalBlock = false) {
        if (!stretcher) return;
        stretcher->process(inPtrs, frames, finalBlock);
    }

    // Retrieve up to maxFrames of processed audio; returns frames actually retrieved
    int retrieve(float** outPtrs, int maxFrames) {
        if (!stretcher) return 0;
        return stretcher->retrieve(outPtrs, maxFrames);
    }

private:
    std::unique_ptr<RubberBandStretcher> stretcher;
    int sampleRate;
    int channels;
};
