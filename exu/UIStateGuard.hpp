//
//  UIStateGuard.hpp
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 12/01/2020.
//

#ifndef UIStateGuard_hpp
#define UIStateGuard_hpp

#include <stdio.h>
#include <JuceHeader.h>
namespace exu{
    class UIStateGuard {
    public:
        UIStateGuard(){}
        ~UIStateGuard(){}
        bool isTouched(){ return touched;}
        void touch(){ touched = true; }
        void unTouch(){ touched = false; }
    private:
        bool touched=true;
    };
}
#endif /* UIStateGuard_hpp */
