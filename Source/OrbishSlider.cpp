//
//  OrbishSlider.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 28/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#include "OrbishSlider.h"


    
OrbishSlider::OrbishSlider(){}
OrbishSlider::OrbishSlider(String s): Slider(s){}

OrbishSlider::~OrbishSlider(){}
    
    int OrbishSlider::getNumDecimalPlacesToDisplay() const noexcept {
        return numDecimalPlaces;
    }

    void OrbishSlider::setNumDecimalPlacesToDisplay (int decimalPlacesToDisplay)
    {
        numDecimalPlaces = decimalPlacesToDisplay;
        updateText();
    }

    String OrbishSlider::getTextFromValue (double value) {
        String v;
        if (getNumDecimalPlacesToDisplay() > 0)
            v = String (value, getNumDecimalPlacesToDisplay());
        else
            v = String (roundToInt (value));
        return  (v) + getTextValueSuffix();
    }

