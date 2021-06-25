//
//  openGLComponentContainer.cpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 13/06/2021.
//  Copyright © 2021 exu. All rights reserved.
//


#include "OpenGLComponentContainer.h"

OpenGLComponentContainer::OpenGLComponentContainer(){
    setOpaque(false);
    setAlpha(1);
}

OpenGLComponentContainer::~OpenGLComponentContainer(){}

void OpenGLComponentContainer::paint(Graphics& g){
    g.setColour(Colours::red);
    g.fillRect(getBounds());
}
