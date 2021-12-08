/*
  ==============================================================================

    OpenGLAudioThumbnail.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLClickWitness.h"
#include "math.h"
//==============================================================================


OpenGLClickWitness::OpenGLClickWitness (std::atomic<float> &offset):
 OpenGLComponent(offset)
{
	setName("clickWitness");

    shaderName = "circle";
}




