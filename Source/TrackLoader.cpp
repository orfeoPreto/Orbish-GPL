//
//  TrackLoader.cpp
//  Orbish - Shared Code
//
//  Created by Duke Quarcoo on 09/11/2021.
//  Copyright © 2021 exu. All rights reserved.
//

#include "TrackLoader.h"

TrackLoader::TrackLoader(OrbishAudioProcessor& processor) :
    processor(processor) {
    context = processor.context;
    this->tracks = &processor.tracks;
}

TrackLoader::~TrackLoader() {

}

bool TrackLoader::loadFromValueTree(ValueTree* tree) {
#if DEBUG_LOG
    context->logMessage("loading tracks");
#endif
    if (tree == nullptr)return false;
    if (!tree->hasType("project"))return false;
    auto tracksElement = tree->getChildWithName("tracks");
    int numTracksToLoad = tracksElement.getNumChildren();
    for (auto i = 0;i < numTracksToLoad;++i) {
        ValueTree tChild(tracksElement.getChild(i));
        auto trackIdx = int(tChild.getProperty("index"));
        if (trackIdx >= 0) {
            if (trackIdx == tracks->size()) {
                processor.processNewTrack();
            }
            Track* t((*tracks)[trackIdx]);
            if (!loadTrackFromValueTree(&tChild, t)) { context->logMessage("File load failed"); }
        }
    }
    //    context->logMessage("loading groups");
    auto groupTree = tree->getChildWithName("groups");
    for (auto groupNode : groupTree) {
        processor.processGroupSelect(groupNode.getProperty("index"));
        for (auto j = 0;j < groupNode.getNumChildren(); ++j) {
            auto groupedTrack = groupNode.getChild(j);
            processor.processAddToGroup(groupedTrack.getProperty("index"));
        }
    }
    if (processor.guiAlive) {
        (context->observer->*(context->observer->updatePlayPosition)) (0, processor.activeTrack->Reverse);
    }
    processor.activeTrack->RegisterLoop(0);
    //    activeTrack->UpdateLoopVisualizer();
    return true;
}

bool TrackLoader::loadTrackFromValueTree(ValueTree* trackTree, Track* track) {
    int numLoopsToLoad = trackTree->getNumChildren() - 1;
    for (auto i = 0;i < numLoopsToLoad;++i) {
        ValueTree lChild(trackTree->getChild(i));
        auto loopIdx = int(lChild.getProperty("index", -1));
        if (loopIdx >= 0) {
            if (loopIdx == track->loops.size()) {
                track->processNewLoop();
            }
            if (!loadLoopFromValueTree(&lChild, track->loops[loopIdx], track))return false;
        }
    }
    auto params = trackTree->getChildWithName("TrackParams");
    *track->state = params;
    return true;
}

bool TrackLoader::loadLoopFromValueTree(ValueTree* loopTree, Loop* loop, Track* track) {
    int numLayersToLoad = loopTree->getNumChildren();
    for (auto i = 0;i < numLayersToLoad;++i) {
        auto lyChild = loopTree->getChild(i);
        auto layerIdx = int(lyChild.getProperty("index", -1));
        if (layerIdx >= 0) {
            if (layerIdx == loop->Layers->size()) {
                loop->AddLayer(true, context);
            }
            std::shared_ptr<Layer> layer(loop->Layers.get()->at(layerIdx));
            String filePath = lyChild.getProperty("file");
            if (filePath.isEmpty())return false;
            File file(filePath);
            auto formatMgr = std::make_unique<AudioFormatManager>();
            formatMgr->registerBasicFormats();
            std::unique_ptr<AudioFormatReader> reader(formatMgr->createReaderFor(file));
            if (reader.get() != nullptr)
            {
                try {
                    std::unique_ptr<AudioSampleBuffer> buffer = std::make_unique<AudioSampleBuffer>(reader->numChannels, (int)reader->lengthInSamples);
                    reader->read(buffer.get(), 0, (int)reader->lengthInSamples, 0, true, true);
                    layer->Buffer->setSize(buffer->getNumChannels(), buffer->getNumSamples());
                    layer->Buffer->clear();
                    for (auto i = 0;i < buffer->getNumChannels();++i) {
                        layer->Buffer->copyFrom(i, 0, *buffer, i, 0, buffer->getNumSamples());
                    }
                    loop->LoopDuration = buffer->getNumSamples();
                }
                catch (int e) {
                    context->logMessage("read of audio file failed: " + String(filePath));
                    context->logMessage("Exception occured:" + String(e));
                }
            }
            layer->makeVisualizationBuffer(loop->LoopDuration);
            loop->setActivePlaybackLayer(layer);
            layer->dirty = true;
        }
    }

    loop->updateFlattenedVisualizationBuffer();
    if (loop->CurrentTop < 0 || loop->CurrentTop >= loop->Layers->size()) {
        loop->activePlaybackLayer = nullptr;
    }
    else {
        loop->activePlaybackLayer = loop->Layers.get()->at(loop->CurrentTop);
    }
    return true;
}

