/*
  ==============================================================================

    ModeControlArea.cpp
    Created: 4 Aug 2020 4:24:10pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModeControlArea.h"

//==============================================================================
ModeControlArea::ModeControlArea(){
    Array<StringArray> buttonRecModeNames;
    buttonRecModeNames.add({ "Overdub", "Record multiple layers within same loop" });
    buttonRecModeNames.add({ "Repeat","Record additional layer while extending loop size repeating previous layers" });
    buttonRecModeNames.add({ "Append","Record new layer while extending without repeating previous layers" });
    buttonRecModeNames.add({ "Overwrite", "overwrite content of current layer with new material, and extend original loop until recording ends" });
    buttonRecModeNames.add({ "Punch","Record while replacing previous material on current layer" });
    for (int i = 0; i < buttonRecModeNames.size(); ++i){
        recModeCombo.addItem(buttonRecModeNames[i][0], i + 1);
    }

    String str = String("Overdub: Record multiple layers within same loop\n") +
        String("Repeat: Record additional layer while extending loop size repeating previous layers\n") +
        String("Append: Record new layer while extending without repeating previous layers") +
        String("Overwrite: overwrite content of current layer with new material, and extend original loop until recording ends\n") +
        String("Punch: Record while replacing previous material on current layer\n");
    recModeCombo.setTooltip(str);

    recModeCombo.setSelectedId(1);
    addAndMakeVisible(recModeCombo);
    addAndMakeVisible(recModeLabel);


    Array<StringArray> buttonSnapModeNames;
    buttonSnapModeNames.add({ "No Sync", "Snap disabled, functionality goes into effect instantly" });
    buttonSnapModeNames.add({ "Bar","Snaps to bar" });
    buttonSnapModeNames.add({ "Beat","Snaps to the beat (bottom of time signature)" });
    for (int i = 0; i < buttonSnapModeNames.size(); ++i){
        snapModeCombo.addItem(buttonSnapModeNames[i][0], i + 1);
    }

    str = String("No Sync: Snap disabled, functionality goes into effect instantly\n") +
        String("Bar: Snaps to bar\n") +
        String("Beat: Snaps to the beat (bottom of time signature)");
    snapModeCombo.setTooltip(str);

    snapModeCombo.setSelectedId(2);
    addAndMakeVisible(snapModeCombo);
    addAndMakeVisible(snapModeLabel);

}

ModeControlArea::~ModeControlArea(){
}

void ModeControlArea::paint (juce::Graphics& g){
    auto bounds = getLocalBounds().reduced(5);
    g.drawRoundedRectangle(bounds.toFloat(), 4.0f, 0.5f);

}

void ModeControlArea::resized(){

    auto bounds = getLocalBounds().reduced(15);

    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    grid.setGap(Grid::Px(10));

    grid.templateRows = { Track(Fr(1)), Track(Fr(3)) };
    grid.templateColumns = { Track(Fr(1)), Track(Fr(1)) };

    grid.items = {
        juce::GridItem(recModeLabel), juce::GridItem(snapModeLabel),
        juce::GridItem(recModeCombo), juce::GridItem(snapModeCombo)
    };

    grid.performLayout(bounds);
}
