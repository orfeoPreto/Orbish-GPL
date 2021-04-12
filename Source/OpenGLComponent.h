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

class OpenGLComponent  :
            public Component,
            public OpenGLRenderer

{
public:
    OpenGLComponent(std::atomic<float> &offset);
    ~OpenGLComponent() override;

    void setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext);
    void paint (juce::Graphics&) override;
    void resized() override;
    void renderOpenGL() override;
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void start();
    void stop();
    void setOffset(float &);
    GLuint createTexture(GLint, GLint, bool isDepth=false);
    void updateScale();
    // OpenGL Variables
    std::shared_ptr<OpenGLContext> openGLContext;
    virtual void setUniforms();
    String shaderName;
    std::unique_ptr<Shader> shader;

protected:
    
    GLuint vbo, vao, ebo, fbo;
    unsigned int texture;
    int counter = 0;
    int frameRate = 0;
    int64 stamp=0, startStamp=0;
    std::unique_ptr<Uniforms> uniforms;
    GLfloat vertices[12];
    GLuint indices[6];
    int runs = 0;
    GLfloat* subImage;
    int width, height;
    std::atomic<float>& offset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent);

};
