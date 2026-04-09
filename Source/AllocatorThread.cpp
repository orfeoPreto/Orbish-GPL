#include "AllocatorThread.h"
#include "Context.h"
#include "Track.h"
#include <chrono>

AllocatorThread::AllocatorThread(std::shared_ptr<OrbishContext>& ctx,
                                 Track*& activeTrack,
                                 std::atomic<Track*>& trackToAdd)
    : context(ctx), activeTrack(activeTrack), trackToAdd(trackToAdd) {}

AllocatorThread::~AllocatorThread() {
    stop();
}

void AllocatorThread::start() {
    running = true;
    thread = std::thread([this] { run(); });
}

void AllocatorThread::stop() {
    running = false;
    if (thread.joinable())
        thread.join();
}

void AllocatorThread::run() {
    File file = File(File::getSpecialLocation(File::userHomeDirectory)).getChildFile("Orbish").getChildFile("Orbish.log");
    auto result = file.create();

    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (context->audioInputsCount > 0) {
            if (context->xchange->resetBuffers.get()) {
                context->xchange->layerQueue->reset();
                context->xchange->readBufferQueue->reset();
                context->xchange->resetBuffers = false;
            }
            if (context->xchange->layerQueue->write_available() > 0) {
                auto l = std::make_shared<Layer>(context->audioInputsCount, context->allocatedLength);
                if (l->Buffer->getNumChannels() == 0
                    || l->Buffer->getNumSamples() < context->sampleRate) {
                    continue;
                }
                context->xchange->layerQueue->push(l);
            }
        }

        if (activeTrack != nullptr && *activeTrack->LoopDuration > 0) {
            if (activeTrack->doBounce) {
                activeTrack->BounceAllHistory();
                activeTrack->doBounce = false;
            }
            for (auto layer : *activeTrack->Layers) {
                if (layer->dirty) break;
                auto length = 0;
                if (activeTrack->loopToBeExtended && layer->Buffer->getNumSamples() < context->allocatedLength) {
                    length = context->allocatedLength;
                }
                else if (layer->Buffer->getNumSamples() > ((*activeTrack->LoopDuration) + context->samplesPerBlock)
                         && !(activeTrack->getRecordMode() > 0 && activeTrack->getRecordMode() < 5)) {
                    length = (*activeTrack->LoopDuration) + context->samplesPerBlock;
                }
                if (length > 0) {
                    layer->Buffer->setSize(context->audioOutputsCount, length, true, true, false);
                }
            }
            activeTrack->loopToBeExtended = false;
        }

        if (nullptr != trackToAdd) {
            trackToAdd = nullptr;
        }

        if (context->loggingActive) {
            int64 stamp = Time::getApproximateMillisecondCounter();
            if (stamp - context->timestamp > 1) {
                context->timestamp = stamp;
                context->flushLogs();
            }
        }

        while (context->xchange->readBufferQueue->write_available() > 0) {
            auto rb = std::make_shared<AudioBuffer<float>>();
            rb->clear();
            rb->setSize(context->audioInputsCount, context->samplesPerBlock);
            if (rb->getNumChannels() == 0
                || rb->getReadPointer(0) == NULL
                || rb->getNumSamples() == 0) {
                continue;
            }
            context->xchange->readBufferQueue->push(rb);
        };

        while (context->xchange->writeGainModifierQueue->write_available() > 0) {
            auto gm = std::make_shared<GainModifier>();
            context->xchange->writeGainModifierQueue->push(gm);
        };

        while (context->xchange->readGainModifierQueue->read_available() > 0) {
            auto gm = context->xchange->readGainModifierQueue->front();
            context->xchange->readGainModifierQueue->pop();
            gm->applyGain();
            gm->buffer = nullptr;
        };

        while (context->xchange->writeMeasureBufferQueue->write_available() > 0) {
            auto mb = std::make_shared<MeasureBuffer>();
            mb->buffer = nullptr;
            context->xchange->writeMeasureBufferQueue->push(mb);
        };

        while (context->xchange->readMeasureBufferQueue->read_available() > 0) {
            auto mb = context->xchange->readMeasureBufferQueue->front();
            context->xchange->readMeasureBufferQueue->pop();
            mb->measure();
        };
    }
}
