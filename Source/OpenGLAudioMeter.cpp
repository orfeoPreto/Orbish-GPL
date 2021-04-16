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
    shaderName = "meterBar";
}

void OpenGLAudioMeter::renderOpenGL() {
    for (channelNumber = 0; channelNumber<2; ++channelNumber) {
        if (channelNumber>0) {
            clearViewport = false;
        }
        OpenGLComponent::renderOpenGL();
    }
}

void OpenGLAudioMeter::updateScale(){
    const float renderingScale = (float) openGLContext->getRenderingScale();
    width = roundToInt(renderingScale * getWidth()  * 0.4);
    height = roundToInt(renderingScale * getHeight());
    if (channelNumber == 1){
        x = roundToInt(renderingScale * getWidth()  * 0.6);
    }else if (channelNumber == 0){
        x = 0;
    }
}

void OpenGLAudioMeter::setUniforms(){
    shader->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
    if (channelNumber == 1){
        shader->uniforms->offset->set ((GLfloat) offset2);
    }else if (channelNumber == 0){
        shader->uniforms->offset->set ((GLfloat) offset);
    }
}
