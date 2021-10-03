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
#include "Loop.h"


#define DRAW_ELEMENTS                    1
#define DRAW_COLOUR                   2
#define DRAW_ELEMENTS_COLOUR             3

using namespace juce;

//==============================================================================
/*
*/

enum WaveDisplayType{
    kFlat = 1,
    kLayered
};

class OpenGLAudioThumbnail  :
public OpenGLComponent

{
public:
    OpenGLAudioThumbnail(std::atomic<float> &offset, bool fraction);
    ~OpenGLAudioThumbnail();


    void renderOpenGL() override;
    void openGLContextClosing() override;
    void setTotalAudioLength(int);
    int getTotalLength() override;
    void setAudio();
    void initVisualizationBuffer();
    void setActiveLayer(GLuint);
    void resetVisualizationBuffers();
    void setUniforms() override;
    void newOpenGLContextCreated() override;
    void clear();
    void setBuffer(std::shared_ptr<AudioSampleBuffer>, GLfloat, int);
    void setDisplayType(WaveDisplayType);
    void init() override;
    void setReverse(bool);
    bool getReverse();
    void setSourceLoop(Loop*);
    
    WaveDisplayType getDisplayType();
    // OpenGL Variables
    int sampleRate = 0;
    String shader2Name;
    void activate();
    void deactivate();
    
    
private:
    bool reverse = true;
    int frameRate = 0;
    std::unique_ptr<Shader> shaderThumbnailWave;
    GLfloat flattenedVisualizationBuffer [BUFFER_READ_SIZE];
    std::vector<GLfloat*> visualizationBuffers;
    GLfloat totalAudioLength=0;
    GLuint layerNumber =0;
    Loop* sourceLoop = nullptr;
    WaveDisplayType display;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLAudioThumbnail);
    std::shared_ptr<AudioBuffer<GLfloat> > readBuffer;
};
