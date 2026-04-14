#pragma once

#include <JuceHeader.h>

class AudioImporter {
public:
    struct Result {
        juce::AudioBuffer<float> buffer;
        double originalSampleRate = 0;
        int originalNumSamples = 0;
        bool success = false;
        juce::String errorMessage;
    };

    /** Load an audio file, resample to targetSampleRate, match numChannels.
        If maxSamples > 0, crop or pad the result to exactly that length. */
    static Result loadAndPrepare(const juce::File& file,
                                 double targetSampleRate,
                                 int numChannels,
                                 int maxSamples = -1);

    static bool isAudioFile(const juce::File& file);
    static bool isAudioFile(const juce::String& path);
};
