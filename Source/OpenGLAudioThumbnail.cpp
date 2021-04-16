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


OpenGLAudioThumbnail::OpenGLAudioThumbnail (std::atomic<float> &offset):
 OpenGLComponent(offset)
{
    shaderName = "thumbnail-playhead";
    shader2Name = "thumbnail-wave";

    shaderThumbnailWave = nullptr;
    readBuffer = std::make_unique<AudioSampleBuffer>(2,0);
}

int OpenGLAudioThumbnail:: getTotalLength(){
    return totalAudioLength;
}

void OpenGLAudioThumbnail::newOpenGLContextCreated() {
    OpenGLComponent::newOpenGLContextCreated();
    shaderThumbnailWave = std::make_unique<Shader>(shader2Name.toStdString().c_str(), openGLContext);
}

void OpenGLAudioThumbnail::setUniforms(){
    OpenGLComponent::setUniforms();
    shader->uniforms->totalScope->set ((GLfloat) getTotalLength());
    float window = 1;
    if(sampleRate>0){
        float seconds = getTotalLength() / sampleRate;
        float chunk = 120/8;
        window = seconds/chunk;
    }
    shader->uniforms->windowForLog->set (window);
    shader->uniforms->reverse->set (reverse);
}

void OpenGLAudioThumbnail::renderOpenGL() {
    OpenGLComponent::renderOpenGL();
    try {
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
                glBindBuffer (GL_ARRAY_BUFFER, vbo);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
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
}

void OpenGLAudioThumbnail::openGLContextClosing() {
    OpenGLComponent::openGLContextClosing();
    shaderThumbnailWave.reset();
}

void OpenGLAudioThumbnail::setTotalAudioLength(int t){
    if(GLfloat(t) != totalAudioLength){
        totalAudioLength = GLfloat(t);
    }
    runs = 0;
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

void OpenGLAudioThumbnail::setActiveLayer(GLuint layer){
    layerNumber = layer+1;
}

void OpenGLAudioThumbnail::setBuffer(std::shared_ptr<AudioSampleBuffer> b, GLfloat length, int layerIndex){
    if(layerIndex < 0)return;
    setTotalAudioLength(length);
    readBuffer = b;
    GLfloat * visualizationBuffer = new GLfloat [BUFFER_READ_SIZE];
    FloatVectorOperations::clear(visualizationBuffer, BUFFER_READ_SIZE);
    auto size = getTotalLength();
    for (auto i=0; i<BUFFER_READ_SIZE; ++i) {
        visualizationBuffer[i] =  readBuffer->getSample(0, float(i)/float(BUFFER_READ_SIZE) * size);
    }
    if (layerIndex >= visualizationBuffers.size()) {
        visualizationBuffers.push_back(visualizationBuffer);
    }else{
        visualizationBuffers[layerIndex] = visualizationBuffer;
    }
}

void OpenGLAudioThumbnail::clear(){
    visualizationBuffers.clear();
    readBuffer.reset();
}
