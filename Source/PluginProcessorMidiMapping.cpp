// PluginProcessorMidiMapping.cpp
// Split compilation unit for OrbishAudioProcessor - MIDI mapping dispatch.

#include "PluginProcessor.h"
#include "MidiMapping.h"
#include <cmath>

// --- Non-parameter action implementations (moved from PluginEditor) ---

void OrbishAudioProcessor::executeTempoUp()
{
    double currentBpm = context->bpm;
    double newBpm = currentBpm + 10.0;
    if (newBpm > 200.0) newBpm = 200.0;

    context->bpm = newBpm;
    for (auto track : tracks) {
        if (track->timeStretcher && track->originalTempo > 0.0) {
            double ratio = track->originalTempo / newBpm;
            track->timeStretcher->setTimeRatio(ratio);
        }
    }
    DBG("Tempo changed to " + String(newBpm) + " BPM");
}

void OrbishAudioProcessor::executeTempoDown()
{
    double currentBpm = context->bpm;
    double newBpm = currentBpm - 10.0;
    if (newBpm < 30.0) newBpm = 30.0;

    context->bpm = newBpm;
    for (auto track : tracks) {
        if (track->timeStretcher && track->originalTempo > 0.0) {
            double ratio = track->originalTempo / newBpm;
            track->timeStretcher->setTimeRatio(ratio);
        }
    }
    DBG("Tempo changed to " + String(newBpm) + " BPM");
}

void OrbishAudioProcessor::executePitchUp()
{
    if (activeTrack == nullptr) return;

    float currentRatio = activeTrack->perTrackPitchRatio;
    double semitones = 12.0 * std::log2((double)currentRatio);
    semitones = std::round(semitones) + 1.0;
    if (semitones > 6.0) semitones = 6.0;

    activeTrack->perTrackPitchRatio = (float)std::pow(2.0, semitones / 12.0);
    DBG("Pitch changed to " + String(semitones) + " semitones");
}

void OrbishAudioProcessor::executePitchDown()
{
    if (activeTrack == nullptr) return;

    float currentRatio = activeTrack->perTrackPitchRatio;
    double semitones = 12.0 * std::log2((double)currentRatio);
    semitones = std::round(semitones) - 1.0;
    if (semitones < -6.0) semitones = -6.0;

    activeTrack->perTrackPitchRatio = (float)std::pow(2.0, semitones / 12.0);
    DBG("Pitch changed to " + String(semitones) + " semitones");
}

// --- Central MIDI → action dispatcher ---

void OrbishAudioProcessor::executeMidiAction(MidiAction action, uint8_t value)
{
    const char* paramID = midiActionToParamID(action);

    if (paramID != nullptr)
    {
        auto* param = parameters.getParameter(paramID);
        if (param == nullptr) return;

        if (dynamic_cast<AudioParameterBool*>(param))
        {
            // For booleans: note-on toggles, CC > 63 = on / CC <= 63 = off
            if (value > 0)
            {
                float current = param->getValue();
                param->setValueNotifyingHost(current > 0.5f ? 0.0f : 1.0f);
            }
        }
        else
        {
            // Float/Int/Choice params: map CC 0-127 to normalized 0.0-1.0
            float normalized = value / 127.0f;
            param->setValueNotifyingHost(normalized);
        }
        return;
    }

    // Non-parameter actions
    switch (action)
    {
        case MidiAction::TempoUp:    executeTempoUp();    break;
        case MidiAction::TempoDown:  executeTempoDown();  break;
        case MidiAction::PitchUp:    executePitchUp();     break;
        case MidiAction::PitchDown:  executePitchDown();   break;
        default: break;
    }
}
