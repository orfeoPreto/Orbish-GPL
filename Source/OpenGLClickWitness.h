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


class OpenGLClickWitness  :
public OpenGLComponent

{
public:
    OpenGLClickWitness(std::atomic<float> &);


private:

    GLuint bpm = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLClickWitness);

};
