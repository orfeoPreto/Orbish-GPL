//
//  openGLComponentContainer.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 13/06/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#ifndef openGLComponentContainer_hpp
#define openGLComponentContainer_hpp
#include <stdio.h>
#include "JuceHeader.h"
#include <stdio.h>
class OpenGLComponentContainer:
public Component{
    
public:
    
    OpenGLComponentContainer();
    ~OpenGLComponentContainer();
    
    
 
    
    void paint(Graphics& g) override;

};

#endif /* OpenGLComponentContainer */
