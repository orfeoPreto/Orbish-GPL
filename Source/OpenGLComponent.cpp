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
    setOpaque(true);
    bgColour = Colour{0xff262626};
//    bgColour = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
}

void OpenGLComponent::setOpenGLContext(std::shared_ptr<OpenGLContext> openGLContext, bool owner){
    this->openGLContext = openGLContext;
    if (owner) {
        this->openGLContext->setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
        this->openGLContext->setRenderer(this);
        this->openGLContext->attachTo(*this);
    }
    topLevelComponent = getTopLevelComponent();
}

void OpenGLComponent::setTopLevelComponent(Component* comp){
    topLevelComponent = comp;
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
    x = roundToInt(renderingScale * x);
    y = roundToInt(renderingScale * y);

    width = roundToInt(renderingScale * getWidth());
    height = roundToInt(renderingScale * getHeight());
}

void OpenGLComponent::newOpenGLContextCreated() {
    shader = std::make_unique<Shader>(shaderName.toStdString().c_str(), openGLContext);
    updateScale();
	openGLContext->extensions.glGenBuffers(1, &vbo);
	openGLContext->extensions.glGenBuffers(1, &ebo);

//    glDisable(GL_TEXTURE_2D);

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
    juce::Point<int> pointOnTopLevel = getPointFromTopLevel(topLevelComponent, juce::Point<int>{-localX,-localY});
    x = pointOnTopLevel.getX() * -1;
    y = topLevelComponent->getHeight() + (pointOnTopLevel.getY()) - getHeight();
    updateScale();
    glViewport(x,y, width, height);
    juce::OpenGLHelpers::enableScissorTest (Rectangle<int>{x,y,width,height});
    glHint(GL_LINE_SMOOTH_HINT, 4);
    glEnable (GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    if(clearViewport){
        OpenGLHelpers::clear(bgColour);
    }else{
        clearViewport = true;
    }
    glDisable (GL_SCISSOR_TEST);
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

int OpenGLComponent::getFrameRate(){
    return frameRate;
}

void OpenGLComponent::openGLContextClosing() {
    shader.reset();
}


void OpenGLComponent::setUniforms(){
    shader->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
    shader->uniforms->offset->set ((GLfloat) offset);
    shader->uniforms->origin->set ((GLfloat) x, (GLfloat) y);
}

void OpenGLComponent::setOffset(float &position){
    offset = position;
}
