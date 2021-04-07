#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include "JuceHeader.h"
#include "GainModifier.h"
#include "Layer.h"


class BufferForVisualisation{
public:
	BufferForVisualisation() {}
	~BufferForVisualisation() {
	}
	std::shared_ptr<AudioBuffer<float> > buffer;
    int numSamples = 0;
    int layerIndex = -1;
};

class MeasureBuffer{
public:
    MeasureBuffer() {}
    ~MeasureBuffer() {
    }
    void measure(){
         source->measureBlock(*buffer);
    }
    
    std::shared_ptr<AudioBuffer<float> > buffer;
    FFAU::LevelMeterSource* source;
};

class DataExchange
{
public:
	DataExchange() {
		writeVisualisationBufferQueue = new boost::lockfree::spsc_queue<std::shared_ptr<BufferForVisualisation>, boost::lockfree::capacity<200> >;
		readVisualisationBufferQueue = new boost::lockfree::spsc_queue<std::shared_ptr<BufferForVisualisation>, boost::lockfree::capacity<200> >;
        logWriteMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >;
        logReadMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >;
        readBufferQueue = new boost::lockfree::spsc_queue<std::shared_ptr<AudioBuffer<float> >, boost::lockfree::capacity<200> >;
        writeGainModifierQueue = new boost::lockfree::spsc_queue<std::shared_ptr<GainModifier>, boost::lockfree::capacity<200> >;
        readGainModifierQueue = new boost::lockfree::spsc_queue<std::shared_ptr<GainModifier>, boost::lockfree::capacity<200> >;
        writeMeasureBufferQueue = new boost::lockfree::spsc_queue<std::shared_ptr<MeasureBuffer>, boost::lockfree::capacity<10> >;
        readMeasureBufferQueue = new boost::lockfree::spsc_queue<std::shared_ptr<MeasureBuffer>, boost::lockfree::capacity<10> >;
        layerQueue = new boost::lockfree::spsc_queue<std::shared_ptr<Layer>, boost::lockfree::capacity<3> >;
	}
	~DataExchange() {

	}

	boost::lockfree::spsc_queue<std::shared_ptr<BufferForVisualisation>, boost::lockfree::capacity<200> >* writeVisualisationBufferQueue;
	boost::lockfree::spsc_queue<std::shared_ptr<BufferForVisualisation>, boost::lockfree::capacity<200> >* readVisualisationBufferQueue;
	bool writeBufferDirty = false;

    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >* logReadMessageQueue;
    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<10000> >* logWriteMessageQueue;

    boost::lockfree::spsc_queue<std::shared_ptr<AudioBuffer<float> >, boost::lockfree::capacity<200> >* readBufferQueue;
    
    boost::lockfree::spsc_queue<std::shared_ptr<MeasureBuffer>, boost::lockfree::capacity<10> >* writeMeasureBufferQueue;

    boost::lockfree::spsc_queue<std::shared_ptr<MeasureBuffer>, boost::lockfree::capacity<10> >* readMeasureBufferQueue;
    
    boost::lockfree::spsc_queue<std::shared_ptr<GainModifier>, boost::lockfree::capacity<200> >* writeGainModifierQueue;
    boost::lockfree::spsc_queue<std::shared_ptr<GainModifier>, boost::lockfree::capacity<200> >* readGainModifierQueue;
    boost::lockfree::spsc_queue<std::shared_ptr<Layer>, boost::lockfree::capacity<3> > * layerQueue;
    Atomic<bool> resetBuffers = false;
};

