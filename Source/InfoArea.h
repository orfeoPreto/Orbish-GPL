/*
  ==============================================================================

    InfoArea.h
    Created: 1 Aug 2020 3:22:24pm
    Author:  lenna

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "exu/Label.hpp"
#include "CustomButton.h"
#include "OpenGLComponents.h"

//==============================================================================
/*
*/
class InfoArea  : public juce::Component
{
public:
    InfoArea();
    ~InfoArea() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setProjectName(String name);

    void setTimeSignature(String timeSig);
    String getTimeSignature();

    void setBeatsPerMinute(String bpm);
    String getBeatsPerMinute();

    void setProgress(String progress);
    String getProgress();

    void setTrackNumber(String trackNumber);
    String getTrackNumber();

    void setLoopNumber(String loopNumber);
    String getLoopNumber();

    void setLayerNumber(String layerNumber);
    String getLayerNumber();

    void setGroupNumber(String groupNumber);
    String getGroupNumber();

    void updateBarWitness(double alpha);
    void updateBeatWitness(double alpha);
    void setGroupColour(Colour);
    void setOpenGLContext(std::shared_ptr<OpenGLContext> ctxt);
    void setSubDivs(std::atomic<float> &subDivs);
    void setWitness(std::shared_ptr<OpenGLClickWitness> witness);
//    void setTimeSigNumerator(int);
//    void setTimeSigDenominator(int);
    std::shared_ptr<OpenGLClickWitness> witness;
    CustomButton clickButton{ "", false, true };
    Slider clickLevelSlider;

private:
    exu::Label projectLabel{};
    exu::Label timeSigLabel;
    exu::Label bpmLabel;
    exu::Label progressLabel;
    exu::Label trackNumberLabel;
    exu::Label loopNumberLabel;
    exu::Label layerNumberLabel;
    exu::Label groupNumberLabel;
    DrawablePath barWitness{};
    DrawablePath beatWitness{};
    ImageComponent logo;
    ImageComponent name;
    std::shared_ptr<OpenGLContext> openGLContext;
    double beatAlpha = 0.2;
    double barAlpha = 0.2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoArea)
};
