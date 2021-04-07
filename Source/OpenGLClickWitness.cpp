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
vertices{
    1.0f,   1.0f,  0.0f,  // Top Right
    1.0f,  -1.0f,  0.0f,  // Bottom Right
    -1.0f, -1.0f,  0.0f,  // Bottom Left
    -1.0f,  1.0f,  0.0f   // Top Left
}
, indices {  0, 1, 3, 1, 2, 3 }
, offset(offset)
{
    shaderWitness = nullptr;
}

void OpenGLClickWitness::setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext){
//    this->openGLContext = std::make_shared<OpenGLContext>();

    this->openGLContext = openGLContext;
    this->openGLContext->setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    this->openGLContext->setRenderer(this);
    this->openGLContext->attachTo(*this);
}

void OpenGLClickWitness::start()
{
    openGLContext->setContinuousRepainting (true);
    openGLContext->setSwapInterval(0);
}

void OpenGLClickWitness::stop()
{
    openGLContext->setContinuousRepainting (false);
}

OpenGLClickWitness::~OpenGLClickWitness()
{
    openGLContext->setContinuousRepainting(false);
    openGLContext->detach();
}

void OpenGLClickWitness::paint (juce::Graphics& g)
{
//    getParentComponent()->repaint();
}

int OpenGLClickWitness:: getTotalLength(){
    return totalAudioLength;
}

void OpenGLClickWitness::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void OpenGLClickWitness::updateScale(){
    const float renderingScale = (float) openGLContext->getRenderingScale();
    width = roundToInt(renderingScale * Component::getWidth());
    height = roundToInt(renderingScale * Component::getHeight());
}

void OpenGLClickWitness::newOpenGLContextCreated() {
    shaderWitness = std::make_unique<Shader>("circle", openGLContext);
    updateScale();
  //  createShaders();
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
}

void OpenGLClickWitness::renderOpenGL() {
    jassert (OpenGLHelpers::isContextActive());
    counter++;
    stamp = Time::getApproximateMillisecondCounter();
    if (stamp - startStamp > 1000) {
        startStamp = stamp;
        frameRate = counter;
        counter = 0;
    }
    updateScale();
    glViewport(0, 0, width, height);
    glHint(GL_SAMPLES, 4);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    //set background colour
    OpenGLHelpers::clear(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    {
        //render playhead
        shaderWitness->use();
        // set up the uniforms for use in shader
        shaderWitness->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
        shaderWitness->uniforms->offset->set ((GLfloat) offset);
    }
    {
        glDisable(GL_DEPTH_TEST);
        // enable alpha blending
//        glEnable (GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void OpenGLClickWitness::setPosition(float &position){
    offset = position;
    DBG("Update pos");
}

void OpenGLClickWitness::openGLContextClosing() {
    shaderWitness.reset();
}

void OpenGLClickWitness::setTotalAudioLength(int t){
    totalAudioLength = GLfloat(t);
    runs = 0;
}

