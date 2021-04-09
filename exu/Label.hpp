//
//  Label.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 12/01/2020.
//

#ifndef Label_hpp
#define Label_hpp

#include <stdio.h>
#include <JuceHeader.h>
#include "../exu/UIStateGuard.hpp"
namespace exu{

    class Label: public juce::Label, public UIStateGuard {
        
    public:
        Label (const String& componentName = String(),
               const String& labelText = String()):  juce::Label(componentName, labelText){
            
        }
        
        void paint(Graphics& g) override{
           if(isTouched()){
                    juce::Label::paint(g);
                    unTouch();
                }
            }
        
    };
}
#endif /* Label_hpp */
