/*
  ==============================================================================

 OpenGLComponent.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLComponent.h"
//==============================================================================


OpenGLComponent::OpenGLComponent (std::atomic<float>& offset):
vertices{
    1.0f,   1.0f,  0.0f,  // Top Right
    1.0f,  -1.0f,  0.0f,  // Bottom Right
    -1.0f, -1.0f,  0.0f,  // Bottom Left
    -1.0f,  1.0f,  0.0f   // Top Left
}
, indices {  0, 1, 3, 1, 2, 3 }
,offset(offset)
{
    shader = nullptr;
}

void OpenGLComponent::setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext){
//    this->openGLContext = std::make_shared<OpenGLContext>();

    this->openGLContext = openGLContext;
    this->openGLContext->setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    this->openGLContext->setRenderer(this);
    this->openGLContext->attachTo(*this);
}

void OpenGLComponent::start()
{
    openGLContext->setContinuousRepainting (true);
    //openGLContext->setSwapInterval(0);
}

void OpenGLComponent::stop()
{
    openGLContext->setContinuousRepainting (false);
}

OpenGLComponent::~OpenGLComponent()
{
    openGLContext->setContinuousRepainting(false);
    openGLContext->detach();
}

void OpenGLComponent::paint (juce::Graphics& g)
{
//    getParentComponent()->repaint();
}



void OpenGLComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void OpenGLComponent::updateScale(){
    const float renderingScale = (float) openGLContext->getRenderingScale();
    width = roundToInt(renderingScale * getWidth());
    height = roundToInt(renderingScale * getHeight());
}

void OpenGLComponent::newOpenGLContextCreated() {
    shader = std::make_unique<Shader>(shaderName.toStdString().c_str(), openGLContext);
    updateScale();
	openGLContext->extensions.glGenBuffers(1, &vbo);
	openGLContext->extensions.glGenBuffers(1, &ebo);
    
}

void OpenGLComponent::renderOpenGL() {
    jassert (OpenGLHelpers::isContextActive());
    counter++;
    stamp = Time::getApproximateMillisecondCounter();
    if (stamp - startStamp > 1000) {
        startStamp = stamp;
        frameRate = counter;
        counter = 0;
    }
    updateScale();
    glViewport(x, y, width, height);
    glHint(GL_LINE_SMOOTH_HINT, 4);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    //set background colour
    if(clearViewport){
        OpenGLHelpers::clear(getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    }else{
        clearViewport = true;
    }
    {
        shader->use();
        try {
            setUniforms();
        } catch (int e) {
            std::cout << "Exception occured:" << e << "\n";
        }
    }
    {
        glDisable(GL_DEPTH_TEST);
		openGLContext->extensions.glBindBuffer (GL_ARRAY_BUFFER, vbo);
		openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
		openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
		openGLContext->extensions.glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
		openGLContext->extensions.glEnableVertexAttribArray(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void OpenGLComponent::openGLContextClosing() {
    shader.reset();
}


void OpenGLComponent::setUniforms(){
    shader->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
    shader->uniforms->offset->set ((GLfloat) offset);
}

void OpenGLComponent::setOffset(float &position){
    offset = position;
}
