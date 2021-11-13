/*
  ==============================================================================

    OpenGLAudioThumbnail.cpp
    Created: 4 Mar 2021 9:10:29pm
    Author:  Duke Quarcoo

  ==============================================================================
*/
#include "OpenGLAudioThumbnail.h"
#include "Orbish.h"
#include <JuceHeader.h>


//==============================================================================


OpenGLAudioThumbnail::OpenGLAudioThumbnail (std::atomic<float> &offset, bool fraction):
 OpenGLComponent(offset, fraction)
{
	setDescription("thumbnail");
    shaderName = "thumbnail-playhead";
    shader2Name = "thumbnail-wave";

    shaderThumbnailWave = nullptr;
    readBuffer = std::make_unique<AudioSampleBuffer>(2,0);
    bgColour = juce::Colour{0xff000011};
//    auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);

//    setAlpha(0);
//    bgColour = Colours::black;
    //toFront(true);
    FloatVectorOperations::clear(flattenedVisualizationBuffer, BUFFER_READ_SIZE);
}

int OpenGLAudioThumbnail:: getTotalLength(){
    return totalAudioLength;
}

void OpenGLAudioThumbnail::init() {
//    auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);

    if(!initialized){
        OpenGLComponent::init();
        shaderThumbnailWave = std::make_unique<Shader>(shader2Name.toStdString().c_str(), openGLContext.get());
    }
}

void OpenGLAudioThumbnail::newOpenGLContextCreated() {
        OpenGLComponent::newOpenGLContextCreated();
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
    if (getDisplayType() == kLayered){
        shader->uniforms->reverse->set (reverse);
    }else{
        shader->uniforms->reverse->set (true);
    }
}

OpenGLAudioThumbnail::~OpenGLAudioThumbnail(){
    clear();
}

void OpenGLAudioThumbnail::activate(){
    OpenGLComponent::bgColour = Colour{0xff000000};
}
void OpenGLAudioThumbnail::deactivate(){
    OpenGLComponent::bgColour = Colour{0xff565656};
}

void OpenGLAudioThumbnail::prepareActivation() {
	logMessage("prepare activation");

	OpenGLComponent::bgColour = Colour{ 0xff333300 + OpenGLComponent::bgColour.getARGB() };
}

void OpenGLAudioThumbnail::unPrepareActivation() {
	logMessage("unprepare activation");

	OpenGLComponent::bgColour = Colour{ OpenGLComponent::bgColour.getARGB() - 0xff333300 };
}


void OpenGLAudioThumbnail::renderOpenGL() {
	logMessage("start OpenGLAudioThumbnail::renderOpenGL()");
	logMessage("displayType: " + String(getDisplayType()));
	logMessage("topLevelComponent: " + this->getTopLevelComponent()->getComponentID());
    if (nullptr == sourceLoop) {
        return;
    }
    OpenGLComponent::renderOpenGL();
    try {
//        auto grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);
        if(nullptr == sourceLoop->Layers
           || sourceLoop->CurrentTop < 0){
			logMessage("no layers :(");
            return;
        }
        if (getDisplayType() == kFlat) {
            shaderThumbnailWave->use();
            // set up the uniforms for use in shader
            shaderThumbnailWave->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
            if (!reverse) {
                for(auto i=0;i<BUFFER_READ_SIZE;++i){
                    flattenedVisualizationBuffer[i] = sourceLoop->flattenedVisualizationBuffer[BUFFER_READ_SIZE-(i+1)];
                }
            }else{
                FloatVectorOperations::copy(flattenedVisualizationBuffer, sourceLoop->flattenedVisualizationBuffer, BUFFER_READ_SIZE);
            }
            shaderThumbnailWave->uniforms->audioSampleData->set(flattenedVisualizationBuffer, BUFFER_READ_SIZE);
            shaderThumbnailWave->uniforms->origin->set ((GLfloat) x, (GLfloat) y);
            shaderThumbnailWave->uniforms->windowForLog->set (GLfloat(-1));
            {
                openGLContext->extensions.glBindBuffer (GL_ARRAY_BUFFER, vbo);
                openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                openGLContext->extensions.glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
                openGLContext->extensions.glEnableVertexAttribArray(0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }else{
            for(int i=std::max(0, int(layerNumber)-numberVisibleLayers); layerNumber<=sourceLoop->Layers->size() && i<layerNumber;++i){
                {
                    GLfloat vb[BUFFER_READ_SIZE];
                    for (auto j=0; j<BUFFER_READ_SIZE; ++j) {
                        vb[j] = sourceLoop->Layers->at(i)->visualizationBuffer[j];
                    }
                    //render waveform
                    shaderThumbnailWave->use();
                    // set up the uniforms for use in shader
                    shaderThumbnailWave->uniforms->resolution->set ((GLfloat) width, (GLfloat) height);
                    shaderThumbnailWave->uniforms->audioSampleData->set (vb, BUFFER_READ_SIZE);
                    shaderThumbnailWave->uniforms->origin->set ((GLfloat) x, (GLfloat) y);
                    float coeff = 0;
                    float threshold = std::min(float(numberVisibleLayers),float(layerNumber));
                    if(threshold==0)threshold=numberVisibleLayers;
                    float normalize = threshold / float(numberVisibleLayers);
                    float h = std::max(1.f,std::fmod(float(i-(layerNumber-threshold)), threshold));
                    coeff = (h/normalize   )*2*.5;
                   // coeff = pow(coeff,(coeff>=0.5)?1./1.5:1.5);
    //                std::cout << "coeff: " << coeff << "\n";
    //                std::cout << "i: " << i << "\n";
                    shaderThumbnailWave->uniforms->windowForLog->set (GLfloat(coeff));
                }
                {
                    openGLContext->extensions.glBindBuffer (GL_ARRAY_BUFFER, vbo);
                    openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
                    openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                    openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
                    openGLContext->extensions.glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0);
                    openGLContext->extensions.glEnableVertexAttribArray(0);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }
        }
//         grbl = std::make_unique<OpenGLShaderProgram> (*openGLContext);

    } catch (int e) {
		logMessage("Exception occured: "  + String(e) );
    }
	logMessage("end OpenGLAudioThumbnail::renderOpenGL()");

}

void OpenGLAudioThumbnail::openGLContextClosing() {
    OpenGLComponent::openGLContextClosing();
    shaderThumbnailWave.reset();
}

void OpenGLAudioThumbnail::setReverse(bool rev){
    reverse = rev;
}

bool OpenGLAudioThumbnail::getReverse(){
    return reverse;
}

void OpenGLAudioThumbnail::setTotalAudioLength(int t){
    if(GLfloat(t) != totalAudioLength){
        totalAudioLength = GLfloat(t);
    }
    runs = 0;
}

void OpenGLAudioThumbnail::resetVisualizationBuffers(){
    if(getDisplayType() == kLayered){
        setTotalAudioLength(0);
    }
}

void OpenGLAudioThumbnail::setActiveLayer(GLuint layer){
    layerNumber = layer+1;
}

void OpenGLAudioThumbnail::clear(){
    visualizationBuffers.clear();
    FloatVectorOperations::clear(flattenedVisualizationBuffer, BUFFER_READ_SIZE);
    readBuffer = nullptr;
}

void OpenGLAudioThumbnail::setDisplayType(WaveDisplayType displayType){
    display = displayType;
}

WaveDisplayType OpenGLAudioThumbnail::getDisplayType(){
    return display;
}

void OpenGLAudioThumbnail::setSourceLoop(Loop* src){
    sourceLoop = src;
    if (nullptr != src){
        setTotalAudioLength(src->LoopDuration);
    }else{
        setTotalAudioLength(0);
    }
}

void OpenGLAudioThumbnail::setNumberVisibleLayers(int nbrVisible){
    numberVisibleLayers = nbrVisible;
}
