/*
  ==============================================================================

    TransportControlArea.h
    Created: 3 Aug 2020 1:54:04pm
    Author:  Aoriseth

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TransportControlArea  : public juce::Component
{
public:
    TransportControlArea();
    ~TransportControlArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    TextButton button{"TestButton"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportControlArea)
};
