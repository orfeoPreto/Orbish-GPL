//
//  FormattedSlider.cpp
//  Loopability
//
//  Created by Duke Quarcoo on 02/01/2020.
//

#include "FormattedSlider.hpp"

FormattedSlider::FormattedSlider ():Slider(){}
FormattedSlider::FormattedSlider(Slider::SliderStyle style, Slider::TextEntryBoxPosition textBoxPosition): Slider(style, textBoxPosition){}
FormattedSlider::~FormattedSlider() {}

String FormattedSlider::getTextFromValue (double value) {
    return Slider::getTextFromValue(1);
}
