/*
  ==============================================================================

    GroupControlArea.h
    Created: 6 Aug 2020 12:38:51pm
    Author:  Lennart Cockx

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CustomButton.h"
#include "exu/Label.hpp"
class OrbishAudioProcessorEditor;

//==============================================================================
/*
*/
class GroupControlArea  : public juce::Component, juce::Button::Listener, juce::ComboBox::Listener
{
public:
    GroupControlArea();
    ~GroupControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    CustomButton addToGroupButton{ "Group", true };
    CustomButton removeFromGroupButton{ "UnGroup", true };
    CustomButton previousGroupButton{ "<", true, true };
    CustomButton nextGroupButton{ ">", true, true };
    exu::Label groupLabel;
    juce::Label activeGroupLabel{ "1" };
    ComboBox groupCombo;

    void setEditor(OrbishAudioProcessorEditor*);

private:
    OrbishAudioProcessorEditor* editor;

    void buttonClicked(Button*) override;
    void comboBoxChanged(ComboBox*) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GroupControlArea)
};
