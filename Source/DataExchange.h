#pragma once
#include <boost/lockfree/spsc_queue.hpp>
#include "JuceHeader.h"

class BufferForVisualisation{
public:
	BufferForVisualisation() {}
	~BufferForVisualisation() {
		delete buffer;
	}
	AudioBuffer<float>* buffer;
	int numSamples = 0;
};

class DataExchange
{
public:
	DataExchange() {
		writeVisualisationBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
		readVisualisationBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
        logWriteMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<1000> >;
        logReadMessageQueue = new boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<1000> >;
        readBufferQueue = new boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<5> >;

	}
	~DataExchange() {

	}

	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* writeVisualisationBufferQueue;
	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* readVisualisationBufferQueue;
	bool writeBufferDirty = false;

    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<1000> >* logReadMessageQueue;
    boost::lockfree::spsc_queue<std::string*, boost::lockfree::capacity<1000> >* logWriteMessageQueue;

    boost::lockfree::spsc_queue<AudioBuffer<float>*, boost::lockfree::capacity<5> >* readBufferQueue;


};

