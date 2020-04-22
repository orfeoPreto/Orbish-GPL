//
//  FormattedSlider.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 02/01/2020.
//

#ifndef FormattedSlider_hpp
#define FormattedSlider_hpp

#include <stdio.h>
#include "../JuceLibraryCode/JuceHeader.h"
#endif /* FormattedSlider_hpp */


class FormattedSlider:public Slider {
public:
    FormattedSlider ();
    FormattedSlider (Slider::SliderStyle style, Slider::TextEntryBoxPosition textBoxPosition);
    ~FormattedSlider() override;
    
    String getTextFromValue (double value) override;
};
