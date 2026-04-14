#include "AudioImporter.h"

static const juce::StringArray supportedExtensions { "wav", "aif", "aiff", "flac", "ogg", "mp3" };

bool AudioImporter::isAudioFile(const juce::File& file) {
    return supportedExtensions.contains(file.getFileExtension().trimCharactersAtStart(".").toLowerCase());
}

bool AudioImporter::isAudioFile(const juce::String& path) {
    return isAudioFile(juce::File(path));
}

AudioImporter::Result AudioImporter::loadAndPrepare(const juce::File& file,
                                                     double targetSampleRate,
                                                     int numChannels,
                                                     int maxSamples) {
    Result result;

    if (!file.existsAsFile()) {
        result.errorMessage = "File does not exist: " + file.getFullPathName();
        return result;
    }

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr) {
        result.errorMessage = "Could not read audio file: " + file.getFileName();
        return result;
    }

    result.originalSampleRate = reader->sampleRate;
    result.originalNumSamples = (int)reader->lengthInSamples;

    if (result.originalNumSamples <= 0) {
        result.errorMessage = "Audio file is empty: " + file.getFileName();
        return result;
    }

    // Read the raw audio
    juce::AudioBuffer<float> rawBuffer((int)reader->numChannels, result.originalNumSamples);
    reader->read(&rawBuffer, 0, result.originalNumSamples, 0, true, true);

    // Resample if needed
    juce::AudioBuffer<float>* sourceBuffer = &rawBuffer;
    juce::AudioBuffer<float> resampledBuffer;

    if (std::abs(reader->sampleRate - targetSampleRate) > 0.01) {
        double ratio = targetSampleRate / reader->sampleRate;
        int newLength = juce::roundToInt(result.originalNumSamples * ratio);
        resampledBuffer.setSize((int)reader->numChannels, newLength);

        for (int ch = 0; ch < (int)reader->numChannels; ++ch) {
            juce::LagrangeInterpolator interpolator;
            interpolator.process(1.0 / ratio,
                                 rawBuffer.getReadPointer(ch),
                                 resampledBuffer.getWritePointer(ch),
                                 newLength);
        }
        sourceBuffer = &resampledBuffer;
    }

    // Determine final length
    int finalLength = sourceBuffer->getNumSamples();
    if (maxSamples > 0)
        finalLength = maxSamples;

    // Build output buffer with correct channel count
    result.buffer.setSize(numChannels, finalLength);
    result.buffer.clear();

    int srcChannels = sourceBuffer->getNumChannels();
    int copyLength = juce::jmin(finalLength, sourceBuffer->getNumSamples());

    if (srcChannels == numChannels) {
        for (int ch = 0; ch < numChannels; ++ch)
            result.buffer.copyFrom(ch, 0, *sourceBuffer, ch, 0, copyLength);
    }
    else if (srcChannels == 1 && numChannels > 1) {
        // Mono to multi: duplicate
        for (int ch = 0; ch < numChannels; ++ch)
            result.buffer.copyFrom(ch, 0, *sourceBuffer, 0, 0, copyLength);
    }
    else if (srcChannels > 1 && numChannels == 1) {
        // Multi to mono: mix down
        for (int ch = 0; ch < srcChannels; ++ch)
            result.buffer.addFrom(0, 0, *sourceBuffer, ch, 0, copyLength, 1.0f / srcChannels);
    }
    else {
        // General case: copy available channels, leave rest silent
        int chToCopy = juce::jmin(srcChannels, numChannels);
        for (int ch = 0; ch < chToCopy; ++ch)
            result.buffer.copyFrom(ch, 0, *sourceBuffer, ch, 0, copyLength);
    }

    result.success = true;
    return result;
}
