/*
  ==============================================================================

    OpenGLComponent.h
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Shader.h"
#include "math.h"
#include "Orbish.h"


using namespace juce;
using namespace juce::gl;

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

class OpenGLComponent  :
            public Component,
            public OpenGLRenderer

{
public:
    OpenGLComponent(std::atomic<float> &offset, bool fraction=false);
    ~OpenGLComponent() override;

    void setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext, bool owner);
    void paint (juce::Graphics&) override;
    void resized() override;
    void renderOpenGL() override;
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void start();
    void stop();
    void setOffset(std::reference_wrapper<std::atomic<float>>);
    GLuint createTexture(GLint, GLint, bool isDepth=false);
    virtual void updateScale();
    // OpenGL Variables
    std::shared_ptr<OpenGLContext> openGLContext;
    virtual void setUniforms();
    String shaderName;
    std::unique_ptr<Shader> shader;
    juce::Rectangle<int> getComponentClippingBoundsRelativeToGLRenderingTarget (juce::Component* targetComponent);
//    int getFrameRate();
//    void setTopLevelComponent(Component*);
    virtual int getTotalLength();
    virtual void init();
    bool isInitialized();

protected:
//    Component* topLevelComponent;
    GLuint vbo, vao, ebo, fbo;
    unsigned int texture;
    std::unique_ptr<Uniforms> uniforms;
    GLfloat vertices[12];
    GLuint indices[6];
    int runs = 0;
    GLfloat* subImage;
    int width=0, height=0, x=0, y=0, localX=0, localY=0;
    std::reference_wrapper<std::atomic<float>> offset;
    bool clearViewport = true;
    Colour bgColour;
    bool fractionOfTotal = false;
    bool initialized = false;
    bool ownsContext = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent);

};
