//
//  MofflRAudioFormatReader.hpp
//  MofflR - VST3
//
//  Created by Duke Quarcoo on 28/04/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#ifndef MofflRAudioFormatReader_h
#define MofflRAudioFormatReader_h

#include <stdio.h>
#include <JuceHeader.h>


class MofflRAudioFormatReader : public AudioFormatReader
{
    public:
    MofflRAudioFormatReader():AudioFormatReader(nullptr, ""){}
    ~MofflRAudioFormatReader(){
        AudioFormatReader::~AudioFormatReader();
    }
    bool readSamples (int** destChannels,
                                               int numDestChannels,
                                               int startOffsetInDestBuffer,
                                               int64 startSampleInFile,
                                               int numSamples){
        // I'm not implementing this
        return false;
    }
};
#endif /* MofflRAudioFormatReader_hpp */
