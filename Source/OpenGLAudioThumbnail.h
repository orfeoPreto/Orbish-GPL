/*
  ==============================================================================

    OpenGLAudioThumbnail.h
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//#include "RingBuffer.h"
#include "Shader.h"

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
            public Component,
            public OpenGLRenderer

{
public:
    OpenGLAudioThumbnail(std::shared_ptr<OpenGLContext> openGLContext);
    ~OpenGLAudioThumbnail() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void renderOpenGL() override;
    
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void createShaders();
    void createRenderShaders();

    void start();
    
    void stop();

    
    void setTotalAudioLength(int);
    int getTotalLength();
    void setOffset(int);
    GLuint createTexture(GLint, GLint, bool isDepth=false);
    void setAudio();
    std::unique_ptr<AudioBuffer<GLfloat> > readBuffer;
    void initVisualizationBuffer();
    void setPlayhead(int position);
    void drawFramebuffer(GLuint, GLuint, GLuint*, WhatToDraw, bool blit=false);
    void createFrameBuffer(GLuint* fb, GLuint tx);
    void setLayer(GLuint);
    void resetVisualizationBuffers();
    void updateScale();
    // OpenGL Variables
    std::shared_ptr<OpenGLContext> openGLContext;
    int sampleRate = 0;
    bool reverse = false;
private:
    
    GLuint vbo, vao, ebo, fbo, fbo2;
    unsigned int texture, texture2;
    int counter = 0;
    int frameRate = 0;
    
    int64 stamp=0, startStamp=0;
    std::unique_ptr<Shader> shaderThumbnailWave, shaderThumbnailPlayhead, renderShader;
    std::unique_ptr<Uniforms> uniforms;
    std::unique_ptr<Uniforms> rUniforms;
    GLfloat vertices[12];
    GLfloat vertices2[12];
    GLuint indices[6];
    GLfloat quadVertices[20];
    GLfloat visualizationBuffer [BUFFER_READ_SIZE];
    std::vector<GLfloat*> visualizationBuffers;
    String statusText;
    //std::unique_ptr<RingBuffer<GLfloat> > ringBuffer;
    GLfloat totalAudioLength=0;
    GLfloat offset=0;
    int runs = 0;
    GLfloat* subImage;
    int width, height;
    GLuint layerNumber =0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAudioThumbnail);

};
