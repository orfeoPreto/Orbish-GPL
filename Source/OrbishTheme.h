/*
  ==============================================================================

    OrbishTheme.h
    Theme definitions for Orbish colour schemes.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum class OrbishThemeId : int {
    ObsidianGold = 0,
    PorcelainInk,
    COUNT
};

struct OrbishThemeColours {
    // Core UI colours
    juce::Colour backgroundOuter;     // outer plugin background
    juce::Colour backgroundInner;     // band/panel inner background
    juce::Colour panelSurface;        // card/panel background (control rail panels)
    juce::Colour buttonSurface;       // button background
    juce::Colour textPrimary;         // main text
    juce::Colour textSecondary;       // dimmer text
    juce::Colour textTertiary;        // muted text (status bar, etc)
    juce::Colour accent;              // primary accent
    juce::Colour accentBright;        // lighter accent (highlights, meter tops)
    juce::Colour accentDim;           // darker accent (gradient bottoms)
    juce::Colour recordAccent;        // record button only
    juce::Colour playhead;            // playhead line
    juce::Colour divider;             // border/divider colour (already includes opacity)

    // Waveform fill (passed to shader as vec3 0-1)
    float waveR, waveG, waveB;

    // Thumbnail backgrounds
    juce::Colour thumbnailBgActive;
    juce::Colour thumbnailBgInactive;

    // Loop progress bars (active track)
    juce::Colour loopActiveCurrentFg;
    juce::Colour loopActiveCurrentBg;
    juce::Colour loopActiveOtherFg;
    juce::Colour loopActiveOtherBg;
    // Loop progress bars (inactive track)
    juce::Colour loopInactiveCurrentFg;
    juce::Colour loopInactiveCurrentBg;
    juce::Colour loopInactiveOtherFg;
    juce::Colour loopInactiveOtherBg;

    // Meter
    juce::Colour meterBg;

    // Settings overlay
    juce::Colour settingsOverlay;

    // Slider
    juce::Colour sliderTrack;
};

inline const char* orbishThemeName(OrbishThemeId id) {
    switch (id) {
        case OrbishThemeId::ObsidianGold:  return "Obsidian & Gold";
        case OrbishThemeId::PorcelainInk:  return "Porcelain & Ink";
        default:                           return "Unknown";
    }
}

inline OrbishThemeColours orbishThemeColours(OrbishThemeId id) {
    using C = juce::Colour;
    switch (id) {
        case OrbishThemeId::ObsidianGold:
        default:
            return {
                // backgroundOuter, backgroundInner, panelSurface, buttonSurface
                C(0xff0b0b0f), C(0xff070709), C(0xff0d0d13), C(0xff16161c),
                // textPrimary, textSecondary, textTertiary
                C(0xffe8e6dc), C(0xff8a877a), C(0xff6b6960),
                // accent, accentBright, accentDim
                C(0xffd4af37), C(0xfff5d76e), C(0xff8a6f1a),
                // recordAccent, playhead, divider
                C(0xffe84545), C(0xb3ffffff), C(0x2ad4af37),
                // waveR, waveG, waveB (gold)
                0.831f, 0.686f, 0.216f,
                // thumbnailBgActive, thumbnailBgInactive
                C(0xff070709), C(0xff16161c),
                // loops: active-current fg/bg, active-other fg/bg
                C(0xffd4af37), C(0xff2a2a30),
                C(0xff8a6f1a), C(0xff16161c),
                // loops: inactive-current fg/bg, inactive-other fg/bg
                C(0xff8a6f1a), C(0xff16161c),
                C(0xff6b6960), C(0xff16161c),
                // meterBg
                C(0xff16161c),
                // settingsOverlay
                C(0xd0070709),
                // sliderTrack
                C(0xff1a1a22)
            };

        case OrbishThemeId::PorcelainInk:
            return {
                // backgroundOuter, backgroundInner, panelSurface, buttonSurface
                C(0xfff4f1ea), C(0xffebe7dd), C(0xfff4f1ea), C(0xffffffff),
                // textPrimary, textSecondary, textTertiary
                C(0xff1a1a22), C(0xff8a857a), C(0xffb0ab9f),
                // accent, accentBright, accentDim
                C(0xff1a1a22), C(0xff2a2a36), C(0xff4a4a56),
                // recordAccent, playhead, divider
                C(0xffc94a3d), C(0xffc94a3d), C(0x201a1a22),
                // waveR, waveG, waveB (ink/dark)
                0.102f, 0.102f, 0.133f,
                // thumbnailBgActive, thumbnailBgInactive
                C(0xffebe7dd), C(0xfff4f1ea),
                // loops: active-current fg/bg, active-other fg/bg
                C(0xff1a1a22), C(0xffdedad0),
                C(0xff8a857a), C(0xffebe7dd),
                // loops: inactive-current fg/bg, inactive-other fg/bg
                C(0xff8a857a), C(0xffebe7dd),
                C(0xffb0ab9f), C(0xffebe7dd),
                // meterBg
                C(0xffdedad0),
                // settingsOverlay
                C(0xd0f4f1ea),
                // sliderTrack
                C(0xffdedad0)
            };
    }
}
