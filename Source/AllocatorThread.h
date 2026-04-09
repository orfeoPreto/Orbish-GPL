#pragma once

#include "JuceHeader.h"
#include <thread>
#include <atomic>

struct OrbishContext;
class Track;

// Background thread that pre-allocates buffers and processes deferred work
// (bounce, layer resizing, gain/measure queue draining, logging).
// Extracted verbatim from the constructor lambda in PluginProcessor.cpp.
class AllocatorThread {
public:
    AllocatorThread(std::shared_ptr<OrbishContext>& ctx,
                    Track*& activeTrack,
                    std::atomic<Track*>& trackToAdd);
    ~AllocatorThread();

    void start();
    void stop();

private:
    void run();

    std::shared_ptr<OrbishContext>& context;
    Track*& activeTrack;
    std::atomic<Track*>& trackToAdd;
    std::atomic<bool> running{false};
    std::thread thread;
};
