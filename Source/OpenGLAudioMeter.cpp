/*
  ==============================================================================

 OpenGLAudioMeter.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include "OpenGLAudioMeter.h"

//==============================================================================


OpenGLAudioMeter::OpenGLAudioMeter (std::atomic<float>& offset, std::atomic<float>& offset2):
OpenGLComponent(offset),
offset2(offset2)
{
	setName("audiometer");

    shaderName = "meterBar";
    bgColour = juce::Colour(0xff707070);
    //getLookAndFeel().findColour(FFAU::LevelMeter::lmMeterBackgroundColour);
}

void OpenGLAudioMeter::renderOpenGL() {
    channelNumber = 1;
    for (channelNumber = 0; channelNumber<2; ++channelNumber) {
        OpenGLComponent::renderOpenGL();
    }
}

void OpenGLAudioMeter::updateScale(){
    OpenGLComponent::updateScale();
    width = roundToInt(width * 0.4);
        if (channelNumber == 0){
            x += width * 1.4;
        }
}

void OpenGLAudioMeter::setUniforms(){
    shader->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
    if (channelNumber == 1){
        shader->uniforms->offset->set ((GLfloat) offset2);
    }else if (channelNumber == 0){
        shader->uniforms->offset->set ((GLfloat) offset.get());
    }
    shader->uniforms->origin->set ((GLfloat) x, (GLfloat) y);
}
