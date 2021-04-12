/*
  ==============================================================================

    OpenGLAudioThumbnail.h
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once

#include "OpenGLComponent.h"


using namespace juce;

//==============================================================================
/*
*/


class OpenGLAudioMeter  :
            public OpenGLComponent

{
public:
    OpenGLAudioMeter(std::atomic<float> &offset);
    // OpenGL Variables
    int sampleRate = 0;

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAudioMeter);

};
