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


using namespace juce;

//==============================================================================
/*
*/

struct vertex{
    vertex(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    float x;
    float y;
    float z;
};

class OpenGLClickWitness  :
            public Component,
            public OpenGLRenderer

{
public:
    OpenGLClickWitness(std::atomic<float> &offset);
    ~OpenGLClickWitness() override;

    void setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext);
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
    void setLayer(GLuint);
    void setPosition(float &position);
    void updateScale();
    void setBpm(int bpm);
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
    std::unique_ptr<Shader> shaderThumbnailWave, shaderWitness, renderShader;
    std::unique_ptr<Uniforms> uniforms;
    GLfloat vertices[12];
    GLfloat vertices2[12];
    GLuint indices[6];
    GLfloat quadVertices[20];
    std::vector<GLfloat*> visualizationBuffers;
    String statusText;
    //std::unique_ptr<RingBuffer<GLfloat> > ringBuffer;
    GLfloat totalAudioLength=0;
    std::atomic<float>& offset;
    int runs = 0;
    GLfloat* subImage;
    int width, height;
    GLuint layerNumber =0;
    GLuint bpm = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLClickWitness);

};
