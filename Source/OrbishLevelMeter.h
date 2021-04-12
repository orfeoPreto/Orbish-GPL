//
//  OrbishLevelMeter.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 08/04/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef OrbishLevelMeter_hpp
#define OrbishLevelMeter_hpp

#include <stdio.h>
#include "Orbish.h"
#include "OpenGLComponents.h"


class OrbishLevelMeter:
    public FFAU::LevelMeter::LevelMeter
{
public:
    OrbishLevelMeter (const MeterFlags type);
    void paint(Graphics&);
    void resized();
    void timerCallback ();
private:
    std::unique_ptr<OpenGLAudioMeter> meterDisplay;
    std::atomic<float> rms;
};

#endif /* OrbishLevelMeter_hpp */
