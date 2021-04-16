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
    OpenGLAudioMeter(std::atomic<float>&, std::atomic<float>&);
    // OpenGL Variables
    int sampleRate = 0;
    void renderOpenGL() override;
    void updateScale() override;
    void setUniforms() override;
private:
    int channelNumber = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAudioMeter);
    std::atomic<float> &offset2;

};
