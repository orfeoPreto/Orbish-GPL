//
//  MidiProcessor.h
//  Orbish - VST3
//
//  Created by Duke Quarcoo on 03/05/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef MidiProcessor_h
#define MidiProcessor_h

#include <stdio.h>
#include <JuceHeader.h>


class MidiProcessor:public MessageListener
{
public:
    MidiProcessor();
    ~MidiProcessor();
    
    void handleMidi();
    void handleMessage(const Message &message) override;
    
};
#endif /* MidiProcessor_h */
