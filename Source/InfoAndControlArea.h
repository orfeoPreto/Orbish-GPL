/*
  ==============================================================================

    InfoAndControlArea.h
    Created: 1 Aug 2020 12:47:40pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "InfoArea.h"
#include "ControlArea.h"
class OrbishAudioProcessorEditor;

//==============================================================================
/*
*/
class InfoAndControlArea  : public juce::Component
{
public:
    InfoAndControlArea();
    ~InfoAndControlArea() override;
     
    void paint (juce::Graphics&) override;
    void resized() override;
//    void setLookAndFeel (LookAndFeel* const newLookAndFeel);

    InfoArea infoArea{};
    ControlArea controlArea{};

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoAndControlArea)
};
