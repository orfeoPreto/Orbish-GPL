#include "ParameterDefinitions.h"
using namespace juce;

float fromDBTo0To1(float start, float end, float dB){
    if(start == dB){
        return .0f;
    }
    if(end == dB){
        return 1.0f;
    }
    if(dB >= 0){
        // linear distribution if dB above 0
        auto r = dB/end;
        r = .25 * r;
        r = .75 + r;
        return r;
    }else{
            // linear distribution if dB below 0
            auto r = (std::abs(start) - std::abs(dB))/std::abs(start);
            r = .75 * r;
            return r;
    }
}

float from0To1toDB(float start, float end, float gain){
    if(.0f == gain){
        return start;
    }
    if(1.0f == gain){
        return end;
    }
    if(gain >= .75f){
        auto r = gain - .75f;
        r = r / .25f;
        r = r * end;
        return r;
    }else{
        // linear distribution if dB below 0
        auto r = gain / .75f;
        r = r * std::abs(start) - std::abs(start);
        return r;
    }
}

AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    // --- Float parameters with dB curves ---

    auto dbRange = [](float min, float max) {
        return NormalisableRange<float>(min, max,
            [](float start, float end, float gain) { return from0To1toDB(start, end, gain); },
            [](float start, float end, float dB)   { return fromDBTo0To1(start, end, dB); });
    };

    auto clickDbRange = NormalisableRange<float>(-120.0f, 6.0f,
        [](float start, float end, float gain) {
            return Decibels::gainToDecibels(gain * Decibels::decibelsToGain(end), start);
        },
        [](float start, float end, float dB) {
            return Decibels::decibelsToGain(dB, start) / Decibels::decibelsToGain(end);
        });

    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID("globalMix", 1), "GlobalMix", dbRange(-60.0f, 12.0f), 0.5f, "db"));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID("clickLevel", 1), "ClickLevel", clickDbRange, 0.5f, "db"));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID("latency", 1), "Latency",
        NormalisableRange<float>(-500.0f, 500.0f), 50.0f, "ms"));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID("inputLevel", 1), "InputLevel", dbRange(-60.0f, 12.0f), 0.5f, "db"));
    params.push_back(std::make_unique<AudioParameterFloat>(
        ParameterID("outputLevel", 1), "OutputLevel", dbRange(-60.0f, 12.0f), 0.5f, "db"));

    // --- Bool parameters ---

    struct BoolDef { const char* id; const char* name; bool meta; };
    const BoolDef bools[] = {
        {"click",           "Click",           true },
        {"record",          "Record",          true },
        {"play",            "Play",            true },
        {"stop",            "Stop",            true },
        {"mute",            "Mute",            true },
        {"solo",            "Solo",            true },
        {"monitor",         "Monitor",         false},
        {"reverse",         "Reverse",         false},
        {"undo",            "Undo",            false},
        {"redo",            "Redo",            false},
        {"reset",           "Reset",           true },
        {"nextTrack",       "NextTrack",       true },
        {"previousTrack",   "PreviousTrack",   true },
        {"newTrack",        "NewTrack",        false},
        {"removeTrack",     "RemoveTrack",     true },
        {"trigger",         "Trigger",         false},
        {"muteAll",         "MuteAll",         true },
        {"stopAll",         "StopAll",         true },
        {"startAll",        "StartAll",        true },
        {"pauseAll",        "PauseAll",        true },
        {"resetAll",        "ResetAll",        true },
        {"nextSnapMode",    "NextSnapMode",    true },
        {"nextRecMode",     "NextRecMode",     true },
        {"incFixed",        "IncFixed",        true },
        {"bounce",          "Bounce",          true },
        {"nextLoop",        "NextLoop",        true },
        {"previousLoop",    "PreviousLoop",    true },
        {"newLoop",         "NewLoop",         false},
        {"removeLoop",      "RemoveLoop",      true },
        {"addToGroup",      "AddToGroup",      true },
        {"removeFromGroup", "RemoveFromGroup", true },
    };
    for (auto& b : bools) {
        if (b.meta)
            params.push_back(std::make_unique<AudioParameterBool>(
                ParameterID(b.id, 1), b.name, false,
                AudioParameterBoolAttributes().withMeta(true)));
        else
            params.push_back(std::make_unique<AudioParameterBool>(
                ParameterID(b.id, 1), b.name, false));
    }

    // --- Choice parameters ---

    params.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID("snap", 1), "Snap",
        StringArray("No Sync", "Measure", "Beat", "Loop", "Host Loop"), 0.2));
    params.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID("mode", 1), "Mode",
        StringArray("Overdub", "Fixed", "Repeat", "Append", "Overwrite", "Punch"), 0));
    params.push_back(std::make_unique<AudioParameterChoice>(
        ParameterID("selectGroup", 1), "SelectGroup",
        StringArray("A", "B", "C", "D", "E", "F", "G", "H", "I", "J"), 0));

    // --- Int parameters ---

    params.push_back(std::make_unique<AudioParameterInt>(
        ParameterID("trackSelect", 1), "Track", 0, 100, 0,
        AudioParameterIntAttributes().withMeta(true)));
    params.push_back(std::make_unique<AudioParameterInt>(
        ParameterID("loopSelect", 1), "Loop", 0, 100, 0,
        AudioParameterIntAttributes().withMeta(true)));
    params.push_back(std::make_unique<AudioParameterInt>(
        ParameterID("fixed", 1), "fixedSizeLength", 1, 32, 0,
        AudioParameterIntAttributes().withMeta(true)));

    return { params.begin(), params.end() };
}
