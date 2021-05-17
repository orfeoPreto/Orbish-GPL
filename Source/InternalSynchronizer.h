/*
  ==============================================================================

    InternalSynchronizer.h
    Created: 18 Nov 2020 11:21:03pm
    Author:  Duke Quarcoo

  ==============================================================================
*/

#pragma once
#include "Synchronizer.h"

class InternalSynchronizer: public Synchronizer{
public:
    InternalSynchronizer(std::shared_ptr<OrbishContext>, int*);
    ~InternalSynchronizer() override;
    
    int getNextSample(SnapMode) override;
    
    void setCurrentPosition(int*);

private:
    int* currentPos=nullptr;
};
