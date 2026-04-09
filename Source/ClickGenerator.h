#pragma once

#include "JuceHeader.h"
#include "ProcessorState.h"
#include "Context.h"

// Self-contained click generator. All methods inline — zero call overhead.
// Owns the ClickState and produces click audio into an output buffer.
class ClickGenerator {
public:
    // Returns the current downbeat value (1.0 on bar start, 0.0 otherwise).
    inline float getDownbeat() const { return state.downbeat.load(); }

    // Process click for the current audio block.
    // Writes click audio into `output` based on beat position from `context`.
    inline void process(std::shared_ptr<OrbishContext>& context, AudioSampleBuffer* output) {
        int targetOffset = 0, sourceOffset = 0;

        auto timeInSamples = int(context->info->getTimeInSamples().orFallback(0));
        auto offsetFromClosestBeat = context->differenceFromClosestBeatInSamples(timeInSamples);

        // Debug: log click state periodically
        static int clickDbgCounter = 0;
        if (++clickDbgCounter >= 200) {
            clickDbgCounter = 0;
            DBG("handleClick: enabled=" + String(context->clickEnabled ? "Y" : "N")
                + " playing=" + String(context->info->getIsPlaying() ? "Y" : "N")
                + " samplesPerBeat=" + String(context->samplesPerBeat)
                + " timeInSamples=" + String(timeInSamples)
                + " offsetFromBeat=" + String(offsetFromClosestBeat)
                + " maxBlock=" + String(context->maxBlockSize)
                + " inProcess=" + String(state.inProcess ? "Y" : "N"));
        }

        if (!context->clickEnabled
            || !context->info->getIsPlaying()
            || (abs(offsetFromClosestBeat) > context->maxBlockSize
                && !state.inProcess)) {
            return;
        }
        // Lock the click source on the first buffer of a multi-buffer click.
        // Re-selecting every buffer would switch between high/low click mid-playback
        // (different lengths -> truncation and amplitude jumps).
        if (!state.inProcess) {
            // Use PPQ position relative to bar start to determine beat within bar.
            // ppqPosition is set to the bar-start PPQ (from host or standalone).
            double barStartPpq = context->info->getPpqPosition().orFallback(0);
            double currentPpq = context->samplesToQuarters(double(timeInSamples));
            double quartersPerBeat = 4.0 / context->timeSigBottom;
            int beatInBar = (quartersPerBeat > 0)
                ? int(floor((currentPpq - barStartPpq) / quartersPerBeat + 0.5)) % context->timeSigTop
                : 0;
            bool isDownbeat = (beatInBar == 0);
            state.activeSource = isDownbeat
                ? context->barStartClickBuffer.get()
                : context->clickBuffer.get();
            state.downbeat.store(isDownbeat ? 1.0f : 0.0f);
        }
        AudioSampleBuffer* source = state.activeSource;
        auto b = context->maxBlockSize;
        auto a = source->getNumSamples();
        // first buffer of a new click
        if (!state.inProcess) {
            targetOffset = (offsetFromClosestBeat < 0)
                ? abs(offsetFromClosestBeat)
                : 0;
            state.inProcess = true;
            state.sourcePosition = 0;
        } else {
            targetOffset = 0;
            if (state.sourcePosition >= a) {
                state.inProcess = false;
                state.sourcePosition = 0;
                return;
            }
        }
        sourceOffset = state.sourcePosition;
        auto samplesToWrite = jmin(b - targetOffset, a - sourceOffset);

        // Clamp to avoid buffer overruns
        if (sourceOffset + samplesToWrite > source->getNumSamples())
            samplesToWrite = source->getNumSamples() - sourceOffset;
        if (targetOffset + samplesToWrite > output->getNumSamples())
            samplesToWrite = output->getNumSamples() - targetOffset;

        if (samplesToWrite > 0) {
            auto channelsToWrite = jmin(source->getNumChannels(), output->getNumChannels());
            for (auto c = 0; c < channelsToWrite; ++c) {
                output->addFrom(c, targetOffset, source->getReadPointer(c, sourceOffset), samplesToWrite, context->clickLevel);
            }
            state.sourcePosition += samplesToWrite;
        }
    }

private:
    ClickState state;
};
