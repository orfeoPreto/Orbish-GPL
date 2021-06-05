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
#include <ff_meters/ff_meters.h>



class OrbishLevelMeter:
    public FFAU::LevelMeter
{
public:
    OrbishLevelMeter (const MeterFlags type);
    void paint(Graphics&);
    void resized();
    void timerCallback ();
    void setOpenGLContext(std::shared_ptr<OpenGLContext> ctxt);
    std::unique_ptr<OpenGLAudioMeter> meterDisplay;

private:
    std::atomic<float> rms, rms2;
    int64 lastRmsUpdate = 0;
    int64 lastRmsUpdate2 = 0;
    std::shared_ptr<OpenGLContext> openGLContext;
};

#endif /* OrbishLevelMeter_hpp */
