//
//  BlinkingButton.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 02/01/2020.
//

#ifndef Blinker_hpp
#define Blinker_hpp

#include <stdio.h>
#include <JuceHeader.h>



struct Blinker :
public Component, public Timer, public SettableTooltipClient
{
    
    enum ColourIds
    {
       // armedColourId             = 0x1000100,
        onColourId                = 0x1000101,
        offColourId               = 0x1000102
    };
    
    enum State
    {
       // armedColourId             = 0x1000100,
        kNeutral            = 0,
        kArmed              = 1,
        kHighlighted       = 2
    };
    
    Blinker()
    {
       startTimer(300);
    }

    void paint(Graphics& g) override
    {
        switch(state){
            case kNeutral:
                g.setColour(findColour(offColourId));
                break;
            case kArmed:
                if( on ) g.setColour(findColour(onColourId));
                else g.setColour(findColour(offColourId));
                break;
            case kHighlighted:
                g.setColour(findColour(onColourId));
                break;
        }

       // g.fillAll();
        g.fillEllipse(0,0, float(getWidth()), float(getHeight()));
    }

    void timerCallback() override
    {
        on = !on;
        repaint();
    }

    bool on = false;
    State state = kNeutral;
};
#endif /* Blinker_hpp */
