#pragma once

#include "JuceHeader.h"

// Pure math: dB ↔ 0-1 conversion with piecewise linear curve.
// No dependency on processor state.
float fromDBTo0To1(float start, float end, float dB);
float from0To1toDB(float start, float end, float gain);

// Builds the complete AudioProcessorValueTreeState parameter layout.
// Called once in the OrbishAudioProcessor constructor.
AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
