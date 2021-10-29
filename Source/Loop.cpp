#include "Loop.h"


Loop::Loop(){}
Loop::Loop(int index):
    Progress(0){
        Index = index;
        Layers = std::make_shared<std::vector<std::shared_ptr<Layer> > >();
        Layers->reserve(100);
        clearFlattenedVisualizationBuffer();
    }

Loop::~Loop()
{
}
    
    
    void Loop::AddLayer(bool incrementTop, std::shared_ptr<OrbishContext> context) {
        LayersReady = false;
        if (!context->xchange->layerQueue->empty()) {
#if DEBUG_LOG
            long start1 = 0, end1 = 0, start2 = 0, end2 = 0;
            start1 = Time::getHighResolutionTicks();
#endif
            std::shared_ptr<Layer> l = nullptr;
            l = context->xchange->layerQueue->front();
            l->index = int(Layers->size());
            Layers->push_back(l);
#if DEBUG_LOG
            end1 = Time::getHighResolutionTicks();
            start2 = Time::getHighResolutionTicks();
#endif
            context->xchange->layerQueue->pop();
#if DEBUG_LOG
            end2 = Time::getHighResolutionTicks();
            context->logMessage("time for Layers->add: " + String(end1 - start1));
            context->logMessage("time for pop_front: " + String(end2 - start2));
#endif
            if (incrementTop) CurrentTop = int(Layers->size()) - 1;
        }
        LayersReady = true;
    }
    
void Loop::updateFlattenedVisualizationBuffer(){
    clearFlattenedVisualizationBuffer();
        for(auto i=0;i<Layers->size() && i<=activePlaybackLayer->index;++i){
            FloatVectorOperations::add(flattenedVisualizationBuffer, Layers->at(i)->visualizationBuffer, BUFFER_READ_SIZE);
        }
    }

void Loop::clearFlattenedVisualizationBuffer(){
    FloatVectorOperations::clear(flattenedVisualizationBuffer, BUFFER_READ_SIZE);
}

