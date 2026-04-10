/*
  ==============================================================================

    InfoAndControlArea.cpp
    Created: 1 Aug 2020 12:47:40pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#include <JuceHeader.h>
#include "InfoAndControlArea.h"
#include "PluginEditor.h"

//==============================================================================
InfoAndControlArea::InfoAndControlArea(){
    addAndMakeVisible(infoArea);
    addAndMakeVisible(controlArea);
//    setOpaque(true);

}

InfoAndControlArea::~InfoAndControlArea(){
}

void InfoAndControlArea::paint (juce::Graphics&){
}

void InfoAndControlArea::resized(){
    auto bounds = getLocalBounds();
    // InfoArea sidebar hidden - its data is shown in the header bar instead
    infoArea.setBounds(0, 0, 0, 0);
    controlArea.setBounds(bounds);
}

//void InfoAndControlArea::setLookAndFeel (LookAndFeel* const newLookAndFeel){
//    Component::setLookAndFeel(nullptr);
//    Component::setLookAndFeel(newLookAndFeel);
//    infoArea.setLookAndFeel(nullptr);
//    infoArea.setLookAndFeel(newLookAndFeel);
//    controlArea.setLookAndFeel(newLookAndFeel);
//}
