#pragma once

#include <JuceHeader.h>
#include "Context.h"
#include "DataExchange.h"
#include "Track.h"
#include "Loop.h"
#include "Layer.h"
#include "Realignment.h"
#include "InternalSynchronizer.h"
#include "TrackGroup.h"
#include <catch_amalgamated.hpp>

// JUCE requires MessageManager for some internal operations.
struct JUCEInitialiser {
    JUCEInitialiser() {
        juce::initialiseJuce_GUI();
    }
    ~JUCEInitialiser() {
        juce::shutdownJuce_GUI();
    }
};

// Lightweight context that does NOT require a full processor.
inline std::shared_ptr<OrbishContext> makeTestContext() {
    auto ctx = std::make_shared<OrbishContext>();
    ctx->sampleRate = 44100;
    ctx->samplesPerBlock = 512;
    ctx->maxBlockSize = 512;
    ctx->audioInputsCount = 2;
    ctx->audioOutputsCount = 2;
    ctx->allocatedLength = 44100 * 120;
    ctx->bpm = 120.0;
    ctx->timeSigTop = 4;
    ctx->timeSigBottom = 4;
    ctx->samplesPerBeat = 22050;
    ctx->samplesPerQuarter = 22050;
    ctx->samplesPerMinute = 44100.0f * 60.0f;
    ctx->secondsPerSample = 1.0f / 44100.0f;
    ctx->fadeTime = 0;
    ctx->fadeInc = 0;
    ctx->defaultLoopLength = 120;
    ctx->xchange = new DataExchange();
    ctx->observer = nullptr;
    return ctx;
}

// Minimal AudioProcessor stub for constructing APVTS.
struct StubProcessor : public juce::AudioProcessor {
    StubProcessor() : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}
    const juce::String getName() const override { return "Stub"; }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};

// Fixture that provides everything needed to construct a Track.
struct MinimalProcessorFixture {
    StubProcessor stubProcessor;
    juce::AudioProcessorValueTreeState params;
    std::shared_ptr<OrbishContext> context;
    bool guiAlive = false;

    MinimalProcessorFixture()
        : params(stubProcessor, nullptr, "TestState", createParameterLayout())
        , context(makeTestContext())
    {}

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout() {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("inputLevel", 1), "InputLevel",
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.7f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("outputLevel", 1), "OutputLevel",
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.5f));
        return { p.begin(), p.end() };
    }

    std::unique_ptr<Track> makeTrack(uint index = 0, bool active = false) {
        return std::make_unique<Track>(index, active, params, context, guiAlive);
    }
};
