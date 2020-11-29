//
//  OrbishSlider.hpp
//  Orbish
//
//  Created by Duke Quarcoo on 28/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#pragma once


#include "JuceHeader.h"

class OrbishSlider: public Slider{
public:
    OrbishSlider();
    OrbishSlider(String) ;
    ~OrbishSlider();
    int getNumDecimalPlacesToDisplay() const noexcept;
    
    void setNumDecimalPlacesToDisplay (int decimalPlacesToDisplay) ;
    
    
    String getTextFromValue (double value) override;
private:
    int numDecimalPlaces;
};
    
