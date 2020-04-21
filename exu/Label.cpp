//
//  Label.cpp
//  Loopability - VST3
//
//  Created by Duke Quarcoo on 12/01/2020.
//

#include <stdio.h>
#include "../exu/Label.hpp"

//    exu::Label::Label (const String& componentName,
//           const String& labelText):  juce::Label(componentName, labelText), UIStateGuard(){
//        
//    }

    void exu::Label::paint(Graphics& g) {
        if(isTouched()){
            juce::Label::paint(g);
            unTouch();
        }
    }
