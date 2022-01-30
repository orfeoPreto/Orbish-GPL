/*
  ==============================================================================

    ModeControlArea.cpp
    Created: 4 Aug 2020 4:24:10pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ModeControlArea.h"
#include "Orbish.h"

//==============================================================================
ModeControlArea::ModeControlArea(){
    Array<StringArray> listRecModeNames;
    listRecModeNames.add({ "Overdub", "Record multiple layers within same loop" });
    listRecModeNames.add({ "Fixed", "Record multiple layers. Fixed loop length" });
    listRecModeNames.add({ "Repeat","Record additional layer while extending loop size repeating previous layers" });
    listRecModeNames.add({ "Append","Record new layer while extending without repeating previous layers" });
    listRecModeNames.add({ "Overwrite", "overwrite content of current layer with new material, and extend original loop until recording ends" });
    listRecModeNames.add({ "Punch","Record while replacing previous material on current layer" });
    for (int i = 0; i < listRecModeNames.size(); ++i){
        recModeCombo.addItem(listRecModeNames[i][0], i + 1);
    }

    String str = String("Overdub: Record multiple layers within same loop\n") +
    String("Fixed: Record multiple layers in fixed length loop\n") +
        String("Repeat: Record additional layer while extending loop size repeating previous layers\n") +
        String("Append: Record new layer while extending without repeating previous layers") +
        String("Overwrite: overwrite content of current layer with new material, and extend original loop until recording ends\n") +
        String("Punch: Record while replacing previous material on current layer\n");
    recModeCombo.setTooltip(str);

    recModeCombo.setSelectedId(1);
    addAndMakeVisible(recModeCombo);
    addAndMakeVisible(recModeLabel);


    Array<StringArray> listSnapModeNames;
    listSnapModeNames.add({ "No Sync", "Snap disabled, functionality goes into effect instantly" });
    listSnapModeNames.add({ "Bar","Snaps to bar" });
    listSnapModeNames.add({ "Beat","Snaps to the beat (bottom of time signature)" });
    listSnapModeNames.add({ "Loop","Snaps to the duration of the loop" });
    listSnapModeNames.add({ "Host Loop","Snaps to the host if looping" });


    for (int i = 0; i < listSnapModeNames.size(); ++i){
        snapModeCombo.addItem(listSnapModeNames[i][0], i + 1);
    }
    listRecModeNames.clear();
    listSnapModeNames.clear();
    str = String("No Sync: Snap disabled, functionality goes into effect instantly\n") +
        String("Bar: Snaps to bar\n") +
        String("Beat: Snaps to the beat (bottom of time signature)\n") +
        String("Loop: Snaps to the duration of the loop\n");
        String( "Host: Snaps to the host if looping" );


    snapModeCombo.setTooltip(str);

    snapModeCombo.setSelectedId(2);
    addAndMakeVisible(snapModeCombo);
    addAndMakeVisible(snapModeLabel);
    
    StringArray listFixedSizeBeats;
    for(auto i=0;i<32;i++){
        auto s = String(i+1);
        listFixedSizeBeats.add(s);
    }

    fixedSizeBeatsCombo.addItemList(listFixedSizeBeats, 1);
    
    str = String("Length of the fixed size loop in number of beats");

    fixedSizeBeatsCombo.setTooltip(str);
    fixedSizeBeatsCombo.setEnabled(false);
//    fixedSizeBeatsCombo.onChange = [this]{
//        comboBoxChanged(&fixedSizeBeatsCombo);
//    };
    recModeCombo.addListener(this);
    snapModeCombo.addListener(this);

    addAndMakeVisible(fixedSizeBeatsCombo);

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
    grid.templateColumns = { Track(Fr(3)), Track(Fr(2)), Track(Fr(3)) };

    grid.items = {
        juce::GridItem(recModeLabel), juce::GridItem(), juce::GridItem(snapModeLabel),
        juce::GridItem(recModeCombo), juce::GridItem(fixedSizeBeatsCombo), juce::GridItem(snapModeCombo)
    };

    grid.performLayout(bounds);
}

void ModeControlArea::comboBoxChanged (ComboBox *comboBoxThatHasChanged){
    if (comboBoxThatHasChanged == &recModeCombo) {
        if(recModeCombo.getSelectedId()==2) {
            fixedSizeBeatsCombo.setEnabled(true);
        }else{
            fixedSizeBeatsCombo.setEnabled(false);
        }
    }
}
