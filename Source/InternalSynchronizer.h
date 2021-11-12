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
    
    int getNextSynchronizationPoint(SnapMode) override;
    
    void setCurrentPosition(int*);

    void setLoopEnd(int*);
    
    void setMasterLoopEnd(int*);


private:
    int* currentPos=nullptr;
    int* loopEnd = 0;
    int* masterLoopEnd = 0;
};
