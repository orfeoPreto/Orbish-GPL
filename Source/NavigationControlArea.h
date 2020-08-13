/*
  ==============================================================================

    NavigationControlArea.h
    Created: 4 Aug 2020 4:24:03pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomButton.h"
#include "exu/Label.hpp"

//==============================================================================
/*
*/
class NavigationControlArea  : public juce::Component {
public:
    NavigationControlArea();
    ~NavigationControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    exu::Label loopLabel{ "Loops: " };
    exu::Label activeLoopLabel{ "1" };
    CustomButton previousLoopButton{ "<", true, true };
    CustomButton nextLoopButton{ ">", true, true };
    CustomButton newLoopButton{ "+", true, true };
    CustomButton removeLoopButton{ "-", true, true };

    exu::Label trackLabel{ "Tracks: " };
    exu::Label activeTrackLabel{ "1" };
    CustomButton previousTrackButton{ "<", true, true };
    CustomButton nextTrackButton{ ">", true, true };
    CustomButton newTrackButton{ "+", true, true };
    CustomButton removeTrackButton{ "-", true, true };

    void setActiveLoop(String);
    void setActiveTrack(String);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NavigationControlArea)
};
