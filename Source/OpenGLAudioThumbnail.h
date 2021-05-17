/*
  ==============================================================================

    OpenGLAudioThumbnail.h
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "OpenGLComponent.h"


#define BUFFER_READ_SIZE 1000
#define DRAW_ELEMENTS                    1
#define DRAW_COLOUR                   2
#define DRAW_ELEMENTS_COLOUR             3

using namespace juce;

//==============================================================================
/*
*/

enum WhatToDraw{
    kElements = 1,
    kColour,
    kElementsColour
};

class OpenGLAudioThumbnail  :
public OpenGLComponent

{
public:
    OpenGLAudioThumbnail(std::atomic<float> &offset);
    ~OpenGLAudioThumbnail();


    void renderOpenGL() override;
    void openGLContextClosing() override;
    void setTotalAudioLength(int);
    int getTotalLength();
    void setAudio();
    void initVisualizationBuffer();
    void setActiveLayer(GLuint);
    void resetVisualizationBuffers();
    // OpenGL Variables
    int sampleRate = 0;
    bool reverse = true;
    String shader2Name;
    void setUniforms() override;
    void newOpenGLContextCreated() override;
    void clear();
    void setBuffer(std::shared_ptr<AudioSampleBuffer>, GLfloat, int);
private:
    
    int frameRate = 0;
    std::unique_ptr<Shader> shaderThumbnailWave;
    GLfloat visualizationBuffer [BUFFER_READ_SIZE];
    std::vector<GLfloat*> visualizationBuffers;
    GLfloat totalAudioLength=0;
    GLuint layerNumber =0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAudioThumbnail);
    std::shared_ptr<AudioBuffer<GLfloat> > readBuffer;

};
