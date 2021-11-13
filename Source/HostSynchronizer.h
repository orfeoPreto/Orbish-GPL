//
//  HostSynchronizer.hpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 18/11/2020.
//  Copyright © 2020 exu. All rights reserved.
//

#ifndef HostSynchronizer_hpp
#define HostSynchronizer_hpp

#include "Synchronizer.h"

class HostSynchronizer: public Synchronizer{
public:
    HostSynchronizer(std::shared_ptr<OrbishContext>);
    ~HostSynchronizer() override;
    
    int getNextSynchronizationPoint(SnapMode) override;

private:
    
};
#endif /* HostSynchronizer_hpp */
