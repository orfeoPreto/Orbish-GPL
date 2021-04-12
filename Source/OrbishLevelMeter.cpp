//
//  OrbishLevelMeter.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 08/04/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#include "OrbishLevelMeter.h"

OrbishLevelMeter::OrbishLevelMeter (const MeterFlags type)
  :
    FFAU::LevelMeter::LevelMeter (type)
{
    rms = 0;
    meterDisplay = std::make_unique<OpenGLAudioMeter>(rms);
    addAndMakeVisible(*meterDisplay);
    meterDisplay->setOpenGLContext(std::make_unique<OpenGLContext>());
    meterDisplay->start();
}

void OrbishLevelMeter::paint(Graphics& g){
    
}
void OrbishLevelMeter::resized(){
    const juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    meterDisplay->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}
void OrbishLevelMeter::timerCallback ()
{
    rms = source->getRMSLevel (0);
}
