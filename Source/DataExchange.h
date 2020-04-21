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
		writeBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
		readBufferQueue = new boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >;
	}
	~DataExchange() {

	}

	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* writeBufferQueue;
	boost::lockfree::spsc_queue<BufferForVisualisation*, boost::lockfree::capacity<3> >* readBufferQueue;
	bool writeBufferDirty = false;


};

