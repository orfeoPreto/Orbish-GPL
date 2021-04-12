/*
  ==============================================================================

 OpenGLAudioMeter.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include "OpenGLAudioMeter.h"

//==============================================================================


OpenGLAudioMeter::OpenGLAudioMeter (std::atomic<float> &offset):
OpenGLComponent(offset)
{
    shaderName = "meterBar";
}



