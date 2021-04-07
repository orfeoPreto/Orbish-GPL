/*
  ==============================================================================

    OpenGLAudioThumbnail.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLAudioThumbnail.h"
#include "Orbish.h"

//==============================================================================


OpenGLAudioThumbnail::OpenGLAudioThumbnail (std::shared_ptr<OpenGLContext> openGLContext):
vertices{
    1.0f,   1.0f,  0.0f,  // Top Right
    1.0f,  -1.0f,  0.0f,  // Bottom Right
    -1.0f, -1.0f,  0.0f,  // Bottom Left
    -1.0f,  1.0f,  0.0f   // Top Left
}
,vertices2{
    1.0f,   1.0f,  0.0f,  // Top Right
    1.0f,  -1.0f,  0.0f,  // Bottom Right
    -1.0f, -1.0f,  0.0f,  // Bottom Left
    -1.0f,  1.0f,  0.0f   // Top Left
}
, indices {  0, 1, 3, 1, 2, 3 }
,quadVertices {
    -1.0f,   1.0f,  0.0f, 0.0, 1.0,  // Top Right
    -1.0f,  -1.0f,  0.0f, 0.0, 0.0, // Bottom Right
    1.0f, -1.0f,  0.0f, 1.0, 0.0, // Bottom Left
    1.0f,  1.0f,  0.0f, 1.0, 1.0 // Top Left
}{
    this->openGLContext = std::make_shared<OpenGLContext>();
    shaderThumbnailWave = nullptr;
    shaderThumbnailPlayhead = nullptr;
    this->openGLContext->setOpenGLVersionRequired(OpenGLContext::OpenGLVersion::openGL3_2);
    this->openGLContext->setRenderer(this);
    this->openGLContext->attachTo(*this);
    readBuffer = std::make_unique<AudioSampleBuffer>(2,0);
}

void OpenGLAudioThumbnail::start()
{
    openGLContext->setContinuousRepainting (true);
    openGLContext->setSwapInterval(0);
}

void OpenGLAudioThumbnail::stop()
{
    openGLContext->setContinuousRepainting (false);
}

OpenGLAudioThumbnail::~OpenGLAudioThumbnail()
{
    openGLContext->setContinuousRepainting(false);
//    openGLContext->detach();
}

void OpenGLAudioThumbnail::paint (juce::Graphics& g)
{
//    getParentComponent()->repaint();
}

int OpenGLAudioThumbnail:: getTotalLength(){
    return totalAudioLength;
}

void OpenGLAudioThumbnail::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void OpenGLAudioThumbnail::updateScale(){
    const float renderingScale = (float) openGLContext->getRenderingScale();
    width = roundToInt(renderingScale * Component::getWidth());
    height = roundToInt(renderingScale * Component::getHeight());
}

void OpenGLAudioThumbnail::newOpenGLContextCreated() {
    shaderThumbnailWave = std::make_unique<Shader>("thumbnail-wave", openGLContext);
    shaderThumbnailPlayhead = std::make_unique<Shader>("thumbnail-playhead", openGLContext);
    renderShader = std::make_unique<Shader>("render", openGLContext);
    updateScale();
  //  createShaders();
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    texture = createTexture(width, height);
  //  texture2 = createTexture(getWidth(), getHeight());
    createFrameBuffer(&fbo, texture);
  //  createFrameBuffer(&fbo2, texture2);
}

void OpenGLAudioThumbnail::renderOpenGL() {
    counter++;
    stamp = Time::getApproximateMillisecondCounter();
    if (stamp - startStamp > 1000) {
        startStamp = stamp;
        frameRate = counter;
        counter = 0;
    }
    jassert (OpenGLHelpers::isContextActive());
 //   if(1<=runs++){ glSwapAPPLE(); return; }
    //setup viewport
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
        shaderThumbnailPlayhead->use();
        // set up the uniforms for use in shader
        shaderThumbnailPlayhead->uniforms->totalScope->set ((GLfloat) getTotalLength());
        shaderThumbnailPlayhead->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
        shaderThumbnailPlayhead->uniforms->offset->set (offset);
        float window = 1;
        if(sampleRate>0){
            float seconds = getTotalLength() / sampleRate;
            float chunk = 120/8;
            window = seconds/chunk;
        }
        shaderThumbnailPlayhead->uniforms->windowForLog->set (window);
        shaderThumbnailPlayhead->uniforms->reverse->set (reverse);
    }
    {
        glDisable(GL_DEPTH_TEST);
        // enable alpha blending
//        glEnable (GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindBuffer (GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    try {
      //  for (int i=layerNumber-1, h=0; h < 5 && i>=0; --i, ++h) {
        for(int i=std::max(0, int(layerNumber)-LAYERS_VISIBLE); layerNumber<=visualizationBuffers.size() && i<layerNumber;++i){
            {
                GLfloat vb[BUFFER_READ_SIZE];
                for (auto j=0; j<BUFFER_READ_SIZE; ++j) {
                    vb[j] = visualizationBuffers.at(i)[j];
                }
                //render waveform
                shaderThumbnailWave->use();
                // set up the uniforms for use in shader
                shaderThumbnailWave->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
                shaderThumbnailWave->uniforms->audioSampleData->set (vb, BUFFER_READ_SIZE);
                float threshold = std::min(float(LAYERS_VISIBLE),float(layerNumber));
                if(threshold==0)threshold=LAYERS_VISIBLE;
                float normalize = threshold / float(LAYERS_VISIBLE);
                float h = std::max(1.f,std::fmod(float(i-(layerNumber-threshold)), threshold));
                float coeff = (h/normalize   )*2*.5;
               // coeff = pow(coeff,(coeff>=0.5)?1./1.5:1.5);
//                std::cout << "coeff: " << coeff << "\n";
//                std::cout << "i: " << i << "\n";

                shaderThumbnailWave->uniforms->windowForLog->set (GLfloat(coeff));
            }
            {
               // glEnable(GL_DEPTH_TEST);

                glBindBuffer (GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STREAM_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
                glEnableVertexAttribArray(0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
    } catch (int e) {
        std::cout << "Exception occured:" << e << "\n";
    }

//    {
//        glDisable(GL_DEPTH_TEST);
//        renderShader->uniforms->tex->set( GLfloat(texture) );
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STREAM_DRAW);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STREAM_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat), nullptr);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*) (sizeof(GLfloat) * 3));
//        glEnableVertexAttribArray(0);
//        glEnableVertexAttribArray(1);
//        renderShader->use();
//        drawFramebuffer(fbo, 0, &ebo, WhatToDraw::kElementsColour, true);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//    }
  /*  {
        std::unique_ptr<LowLevelGraphicsContext> lowLevelContext(
            createOpenGLGraphicsContext(
                *OpenGLContext::getCurrentContext(),
                getParentComponent()->getWidth(),
                getParentComponent()->getHeight() 
            )
        );

        Graphics g(*lowLevelContext);


        getParentComponent()->paintEntireComponent(g, false);

    } */
}

void OpenGLAudioThumbnail::drawFramebuffer(GLuint source, GLuint target, GLuint* elements, WhatToDraw draws, bool blit){
    if (source > 0){
        glBindFramebuffer(GL_READ_FRAMEBUFFER, source);
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
    if (draws & WhatToDraw::kElements) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, elements);
    }
    if (draws & WhatToDraw::kColour) {
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    if (blit){
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
}

void OpenGLAudioThumbnail::setPlayhead(int position){
    offset = GLfloat(position);
}


void OpenGLAudioThumbnail::openGLContextClosing() {
    shaderThumbnailWave.reset();
    shaderThumbnailPlayhead.reset();

}

void OpenGLAudioThumbnail::setTotalAudioLength(int t){
    totalAudioLength = GLfloat(t);
    runs = 0;
    
}

GLuint OpenGLAudioThumbnail::createTexture(GLint w, GLint h, bool ){
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h ,0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    int i;
    i=glGetError();
    if(i!=0){
        std::cout << "Error happened while loading texture: "<<
        i << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

void OpenGLAudioThumbnail::createFrameBuffer(GLuint* fb, GLuint tx){
    glGenFramebuffers(1, fb);
    glBindFramebuffer(GL_FRAMEBUFFER, *fb);
    if (tx>0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tx, 0);
    }
    int i=glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (i!=GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer status not ok, status=" << i << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void OpenGLAudioThumbnail::resetVisualizationBuffers(){
    visualizationBuffers.clear();
}

void OpenGLAudioThumbnail::initVisualizationBuffer(){
    GLfloat * visualizationBuffer = new GLfloat [BUFFER_READ_SIZE];
    FloatVectorOperations::clear(visualizationBuffer, BUFFER_READ_SIZE);
    auto size = getTotalLength();
    for (auto i=0; i<BUFFER_READ_SIZE; ++i) {
        visualizationBuffer[i] =  readBuffer->getSample(0, float(i)/float(BUFFER_READ_SIZE) * size);
    }
    visualizationBuffers.push_back(visualizationBuffer);
}

void OpenGLAudioThumbnail::setLayer(GLuint layer){
    layerNumber = layer+1;
}
