/*
  ==============================================================================

    ButtonControlArea.cpp
    Created: 3 Aug 2020 2:12:34pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ButtonControlArea.h"
#include "GroupControlArea.h"

//==============================================================================
ButtonControlArea::ButtonControlArea(){
    addAndMakeVisible(transportControlArea);
    addAndMakeVisible(modeAndNavigationControlArea);
    addAndMakeVisible(globalControlArea);
    addAndMakeVisible(outputControlArea);
    addAndMakeVisible(inputControlArea);

    // Reparent utility strip buttons and readouts from globalControlArea
    addAndMakeVisible(globalControlArea.tracksLayoutLeft);
    addAndMakeVisible(globalControlArea.tracksLayoutRight);
    addAndMakeVisible(globalControlArea.tempoDownButton);
    addAndMakeVisible(globalControlArea.tempoUpButton);
    addAndMakeVisible(globalControlArea.bpmReadout);
    addAndMakeVisible(globalControlArea.pitchDownButton);
    addAndMakeVisible(globalControlArea.pitchUpButton);
    addAndMakeVisible(globalControlArea.pitchReadout);
    addAndMakeVisible(globalControlArea.midiLearnButton);
}

ButtonControlArea::~ButtonControlArea(){
}

void ButtonControlArea::adoptClickControls(OpenGLClickWitness* witness, CustomButton* clickBtn) {
    clickWitness = witness;
    clickButton = clickBtn;

    if (clickWitness) addAndMakeVisible(clickWitness);
    if (clickButton) addAndMakeVisible(clickButton);
}

void ButtonControlArea::adoptGroupControls(GroupControlArea* groupArea) {
    groupControls = groupArea;
    if (groupControls == nullptr) return;

    addAndMakeVisible(groupControls->previousGroupButton);
    addAndMakeVisible(groupControls->activeGroupLabel);
    addAndMakeVisible(groupControls->nextGroupButton);
    addAndMakeVisible(groupControls->addToGroupButton);
    addAndMakeVisible(groupControls->removeFromGroupButton);
}

void ButtonControlArea::paint (juce::Graphics& g){
    auto accent = findColour(juce::TextButton::ColourIds::buttonOnColourId);
    auto btnSurf = findColour(juce::TextButton::ColourIds::buttonColourId);

    auto drawPanel = [&](juce::Rectangle<int> area) {
        if (area.isEmpty()) return;
        auto panelBounds = area.reduced(4).toFloat();
        g.setColour(btnSurf.withAlpha(0.25f));
        g.fillRoundedRectangle(panelBounds, 10.0f);
        g.setColour(accent.withAlpha(0.08f));
        g.drawRoundedRectangle(panelBounds, 10.0f, 0.5f);
    };

    // Left panel: Active Track
    drawPanel(transportControlArea.getBoundsInParent());

    // Middle panel: Modes & Nav
    drawPanel(modeAndNavigationControlArea.getBoundsInParent());

    // Right panel: Levels (input + output)
    auto rightBounds = inputControlArea.getBoundsInParent();
    rightBounds.enlargeIfAdjacent(outputControlArea.getBoundsInParent());
    drawPanel(rightBounds);

    // Utility strip background
    auto utilityY = transportControlArea.getBottom() + 6;
    if (utilityY < getHeight()) {
        auto utilBounds = juce::Rectangle<int>(0, utilityY, getWidth(), getHeight() - utilityY)
                              .reduced(4, 2).toFloat();
        g.setColour(btnSurf.withAlpha(0.18f));
        g.fillRoundedRectangle(utilBounds, 8.0f);
        g.setColour(accent.withAlpha(0.06f));
        g.drawRoundedRectangle(utilBounds, 8.0f, 0.5f);
    }
}

void ButtonControlArea::resized(){
    auto bounds = getLocalBounds();

    // Reserve utility strip at bottom (44px) with top gap for visual separation
    bounds.removeFromBottom(6);
    auto utilityHeight = 44;
    auto utilityBounds = bounds.removeFromBottom(utilityHeight);

    // Global controls area hidden (buttons reparented)
    globalControlArea.setBounds(0, 0, 0, 0);

    // Main control rail
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;

    grid.columnGap = juce::Grid::Px(14);
    grid.templateRows = { Track(Fr(1)) };
    grid.templateColumns = { Track(Fr(8)), Track(Fr(5)), Track(Fr(2)), Track(Fr(2)) };

    grid.items = {
        juce::GridItem(transportControlArea),
        juce::GridItem(modeAndNavigationControlArea),
        juce::GridItem(inputControlArea),
        juce::GridItem(outputControlArea)
    };

    grid.performLayout(bounds);

    // Utility strip layout
    auto strip = utilityBounds.reduced(10, 4);
    auto btnH = strip.getHeight();
    auto btnW = 36;

    // Left: Click witness + click toggle
    if (clickWitness) {
        clickWitness->setBounds(strip.removeFromLeft(btnH).reduced(2));
    }
    if (clickButton) {
        clickButton->setBounds(strip.removeFromLeft(btnW).reduced(1, 2));
    }
    strip.removeFromLeft(10);

    // Tempo: down | BPM readout | up
    globalControlArea.tempoDownButton.setBounds(strip.removeFromLeft(btnW).reduced(1, 2));
    globalControlArea.bpmReadout.setBounds(strip.removeFromLeft(52).reduced(0, 2));
    globalControlArea.tempoUpButton.setBounds(strip.removeFromLeft(btnW).reduced(1, 2));
    strip.removeFromLeft(8);

    // Pitch: down | semitone readout | up
    globalControlArea.pitchDownButton.setBounds(strip.removeFromLeft(btnW).reduced(1, 2));
    globalControlArea.pitchReadout.setBounds(strip.removeFromLeft(44).reduced(0, 2));
    globalControlArea.pitchUpButton.setBounds(strip.removeFromLeft(btnW).reduced(1, 2));

    // Right side: Layout toggles + MIDI Learn
    globalControlArea.midiLearnButton.setBounds(strip.removeFromRight(btnW + 30).reduced(1, 2));
    strip.removeFromRight(6);
    globalControlArea.tracksLayoutRight.setBounds(strip.removeFromRight(btnW).reduced(1, 2));
    globalControlArea.tracksLayoutLeft.setBounds(strip.removeFromRight(btnW).reduced(1, 2));

    // Centre: Group nav controls (between pitch and layout)
    if (groupControls != nullptr) {
        auto navW = 28;
        auto labelW = 48;
        auto gBtnW = 32;
        auto totalW = navW * 2 + labelW + gBtnW * 2 + 12;
        auto cx = strip.getX() + (strip.getWidth() - totalW) / 2;
        auto cy = strip.getY();
        auto ch = strip.getHeight();

        groupControls->previousGroupButton.setBounds(cx, cy + 2, navW, ch - 4);
        groupControls->activeGroupLabel.setBounds(cx + navW, cy + 2, labelW, ch - 4);
        groupControls->nextGroupButton.setBounds(cx + navW + labelW, cy + 2, navW, ch - 4);
        groupControls->addToGroupButton.setBounds(cx + navW * 2 + labelW + 6, cy + 2, gBtnW, ch - 4);
        groupControls->removeFromGroupButton.setBounds(cx + navW * 2 + labelW + 6 + gBtnW, cy + 2, gBtnW, ch - 4);
    }
}
