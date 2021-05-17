//
//  Synchronizer.hpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 18/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#ifndef Synchronizer_hpp
#define Synchronizer_hpp

#include "Context.h"
#include <stdio.h>

class Synchronizer{
public:
    Synchronizer(){}
    Synchronizer(std::shared_ptr<OrbishContext> context){
        this->context = context;
    }
    virtual ~Synchronizer(){}
    
    virtual int getNextSample(SnapMode)=0;
    
    std::shared_ptr<OrbishContext> context;

};

#endif /* Synchronizer_hpp */
