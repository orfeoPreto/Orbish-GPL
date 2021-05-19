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
    meterDisplay = std::make_unique<OpenGLAudioMeter>(rms, rms2);
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
void OrbishLevelMeter::timerCallback()
{
	auto newRMS = source->getRMSLevel(0);


	int64 stamp = Time::getApproximateMillisecondCounter();
	if (stamp - lastRmsUpdate > source->getMaxHoldMS() || newRMS < rms) {
		if (stamp - lastRmsUpdate <= source->getMaxHoldMS()) {
			lastRmsUpdate = stamp;
		}
		lastRmsUpdate = stamp;
		rms = source->getRMSLevel(0);
	}
	if (source->getNumChannels() > 1){
		auto newRMS2 = source->getRMSLevel(1);
		if (stamp - lastRmsUpdate2 > source->getMaxHoldMS() || newRMS2 < rms2) {
			lastRmsUpdate2 = stamp;
			rms2 = source->getRMSLevel(1);
		}
	}
}
