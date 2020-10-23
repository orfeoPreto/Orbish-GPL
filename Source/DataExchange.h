#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include "JuceHeader.h"
#include "GainModifier.h"


class BufferForVisualisation{
public:
	BufferForVisualisation() {}
	~BufferForVisualisation() {
		delete buffer;
	}
	AudioBuffer<float>* buffer;
    int numSamples = 0;
};

class MeasureBuffer{
public:
    MeasureBuffer() {}
    ~MeasureBuffer() {
    }
    void measure(){
         source->measureBlock(*buffer);
    }
    
    AudioBuffer<float>* buffer;
    FFAU::LevelMeterSource* source;
};

class DataExchange
{
public:
	DataExchange() {
		writeVisualisationBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
		readVisualisationBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
        logWriteMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >;
        logReadMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >;
        readBufferQueue = new boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<200> >;
        deleteBufferQueue = new boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<200> >;
        writeGainModifierQueue = new boost::lockfree::spsc_queue<GainModifier*, boost::lockfree::capacity<200> >;
        readGainModifierQueue = new boost::lockfree::spsc_queue<GainModifier*, boost::lockfree::capacity<200> >;
        writeMeasureBufferQueue = new boost::lockfree::spsc_queue<MeasureBuffer*, boost::lockfree::capacity<10> >;
        readMeasureBufferQueue = new boost::lockfree::spsc_queue<MeasureBuffer*, boost::lockfree::capacity<10> >;
	}
	~DataExchange() {

	}

	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* writeVisualisationBufferQueue;
	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* readVisualisationBufferQueue;
	bool writeBufferDirty = false;

    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >* logReadMessageQueue;
    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >* logWriteMessageQueue;

    boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<200> >* readBufferQueue;
    boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<200> >* deleteBufferQueue;

    
    boost::lockfree::spsc_queue<MeasureBuffer*, boost::lockfree::capacity<10> >* writeMeasureBufferQueue;

    boost::lockfree::spsc_queue<MeasureBuffer*, boost::lockfree::capacity<10> >* readMeasureBufferQueue;
    
    boost::lockfree::spsc_queue<GainModifier*, boost::lockfree::capacity<200> >* writeGainModifierQueue;
    boost::lockfree::spsc_queue<GainModifier*, boost::lockfree::capacity<200> >* readGainModifierQueue;
};

