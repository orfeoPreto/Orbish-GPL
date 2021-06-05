//
//  OpenGLComponents.h
//  Orbish
//
//  Created by Duke Quarcoo on 23/03/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef OpenGLComponents_h
#define OpenGLComponents_h
#include <JuceHeader.h>
#include "OpenGLAudioThumbnail.h"
#include "OpenGLClickWitness.h"
#include "OpenGLAudioMeter.h"

struct OpenGLComponentReference{
    OpenGLComponentReference (OpenGLComponent* oc){
        asRenderer = dynamic_cast<OpenGLRenderer *>(oc);
        asComponent = dynamic_cast<Component *>(oc);
    }
    OpenGLRenderer* asRenderer;
    Component* asComponent;
};

static inline std::shared_ptr<OpenGLContext> makeOpenGLContext(bool enablePaint, bool shouldContinuouslyRepaint, int swapInterval=0){
    auto openGLContext =  std::make_shared<OpenGLContext> ();
    openGLContext->setComponentPaintingEnabled(enablePaint);
    openGLContext->setSwapInterval(swapInterval);
    openGLContext->setContinuousRepainting(shouldContinuouslyRepaint);
    return openGLContext;
}

#endif /* OpenGLComponents_h */
