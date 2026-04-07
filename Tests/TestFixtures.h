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
        // Float parameters
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("globalMix", 1), "GlobalMix",
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.0f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("clickLevel", 1), "ClickLevel",
            juce::NormalisableRange<float>(-120.0f, 6.0f), 0.0f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("latency", 1), "Latency",
            juce::NormalisableRange<float>(-500.0f, 500.0f), 0.0f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("inputLevel", 1), "InputLevel",
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.7f));
        p.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("outputLevel", 1), "OutputLevel",
            juce::NormalisableRange<float>(-60.0f, 12.0f), 0.5f));
        // Bool parameters
        auto addBool = [&](const char* id, const char* name) {
            p.push_back(std::make_unique<juce::AudioParameterBool>(
                juce::ParameterID(id, 1), name, false));
        };
        addBool("click", "Click");
        addBool("record", "Record");
        addBool("play", "Play");
        addBool("stop", "Stop");
        addBool("mute", "Mute");
        addBool("solo", "Solo");
        addBool("monitor", "Monitor");
        addBool("reverse", "Reverse");
        addBool("undo", "Undo");
        addBool("redo", "Redo");
        addBool("reset", "Reset");
        addBool("nextTrack", "NextTrack");
        addBool("previousTrack", "PreviousTrack");
        addBool("newTrack", "NewTrack");
        addBool("removeTrack", "RemoveTrack");
        addBool("trigger", "Trigger");
        addBool("muteAll", "MuteAll");
        addBool("stopAll", "StopAll");
        addBool("startAll", "StartAll");
        addBool("pauseAll", "PauseAll");
        addBool("resetAll", "ResetAll");
        addBool("nextSnapMode", "NextSnapMode");
        addBool("nextRecMode", "NextRecMode");
        addBool("incFixed", "IncFixed");
        addBool("bounce", "Bounce");
        addBool("nextLoop", "NextLoop");
        addBool("previousLoop", "PreviousLoop");
        addBool("newLoop", "NewLoop");
        addBool("removeLoop", "RemoveLoop");
        addBool("addToGroup", "AddToGroup");
        addBool("removeFromGroup", "RemoveFromGroup");
        // Choice parameters
        p.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("snap", 1), "Snap",
            juce::StringArray("No Sync", "Measure", "Beat", "Loop", "Host Loop"), 0));
        p.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("mode", 1), "Mode",
            juce::StringArray("Overdub", "Fixed", "Repeat", "Append", "Overwrite", "Punch"), 0));
        p.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID("selectGroup", 1), "SelectGroup",
            juce::StringArray("A", "B", "C", "D", "E", "F", "G", "H", "I", "J"), 0));
        // Int parameters
        p.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("trackSelect", 1), "Track", 0, 100, 0));
        p.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("loopSelect", 1), "Loop", 0, 100, 0));
        p.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("fixed", 1), "fixedSizeLength", 1, 32, 1));
        return { p.begin(), p.end() };
    }

    std::unique_ptr<Track> makeTrack(uint index = 0, bool active = false) {
        return std::make_unique<Track>(index, active, params, context, guiAlive);
    }
};
