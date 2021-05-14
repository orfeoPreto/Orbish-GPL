//
//  Track.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 23/03/2019.
//  Copyright © 2019 EXU. All rights reserved.
//

#include "Track.h"

#include <math.h>
#include <mutex>



Track::Track(uint index, bool a, AudioProcessorValueTreeState& p, OrbishContext* &c, bool& gui) : guiAlive(gui)
, refresh(false)
, params(p)
, context(c){
    Index = index;
    RunAfters.reserve(10);
    int foo =0;
    CurrentPlayingIndex = &foo;
    primarySynchronizer = std::make_unique<InternalSynchronizer>(context, CurrentPlayingIndex);
    secondarySynchronizer = std::make_unique<InternalSynchronizer>(context, &CurrentRecordingIndex);

    state = new ValueTree(
                          { "TrackParams", {{ "index", int(index) }},
        {
            { "Parameter", {{ "id", "record" }, { "value", false }}},
            { "Parameter", {{ "id", "play" },     { "value", false }}},
            { "Parameter", {{ "id", "stop" },     { "value", false }}},
            { "Parameter", {{ "id", "reverse" },     { "value", false }}},
            { "Parameter", {{ "id", "trigger" },     { "value", false }}},
            { "Parameter", {{ "id", "reset" },     { "value", false }}},
            { "Parameter", {{ "id", "monitor" },     { "value", false }}},
            { "Parameter", {{ "id", "mute" },     { "value", false }}},
            { "Parameter", {{ "id", "solo" },     { "value", false }}},
            { "Parameter", {{ "id", "undo" },     { "value", false }}},
            { "Parameter", {{ "id", "redo" },     { "value", false }}},
            { "Parameter", {{ "id", "inputLevel" },     { "value", 0.7 }}},
            { "Parameter", {{ "id", "outputLevel" },     { "value", 0.5 }}},
            { "Parameter", {{ "id", "mode" },     { "value", 0 }}},
            { "Parameter", {{ "id", "snap" },     { "value", 1 }}},
            { "Parameter", {{ "id", "bounce" },     { "value", 1 }}},
            { "Parameter", {{ "id", "nextLoop" }, { "value" , false}}},
            { "Parameter", {{ "id", "previousLoop" }, { "value" , false}}},
            { "Parameter", {{ "id", "newLoop" }, { "value" , false}}},
            { "Parameter", {{ "id", "removeLoop" }, { "value" , false}}},
            { "Parameter", {{ "id", "loopSelect" }, { "value" , 0 }}},
            { "Parameter", {{ "id", "fixed" }, { "value" , 0 }}}
        }
    });
    
    state->setProperty("inputLevel", 0.7f, nullptr);
    state->setProperty("outputLevel", 0.5f, nullptr);
    state->setProperty("record", false, nullptr);
    state->setProperty("play", false, nullptr);
    state->setProperty("stop", false, nullptr);
    state->setProperty("reset", false, nullptr);
    state->setProperty("reverse", false, nullptr);
    state->setProperty("monitor", true, nullptr);
    state->setProperty("mute", false, nullptr);
    state->setProperty("solo", false, nullptr);
    state->setProperty("trigger", false, nullptr);
    state->setProperty("mode", 0, nullptr);
    state->setProperty("snap", 1, nullptr);
    state->setProperty("bounce", 1, nullptr);
    state->setProperty("nextLoop", false, nullptr);
    state->setProperty("previousLoop", false, nullptr);
    state->setProperty("newLoop", false, nullptr);
    state->setProperty("removeLoop", false, nullptr);
    state->setProperty("loopSelect", 0, nullptr);
    state->setProperty("fixed", 0, nullptr);

    
    setActive(a);
    setPlayArmed(false);
    setReverseArmed(false);
    setRecordingArmed(false);
    setMuteArmed(false);
    setSoloArmed(false);
    setMonitoring(true);
    setAutoTrigger(false);
    AddLoop();
    RegisterLoop(loops.size()-1);
    realignment = new Realignment(context->samplesPerBlock, context->sampleRate);
}

void Track::setState(ValueTree* inputState){
    
}

Track::~Track() {
    
    RemoveAllLayers();
    setActive(false);
}

int Track::getNextSample(SnapMode snapMode){
    if(Recording){
        return secondarySynchronizer->getNextSample(snapMode);
    }else if(Playing && *LoopDuration > 0){
        return primarySynchronizer->getNextSample(snapMode);
    }
    return -1;
}

bool Track::isActive(){
    return active;
}

void Track::setActive(bool newValue){
    if(newValue){
        if(!isActive()){
            auto* p = params.getParameter("record");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("record")));
            p = params.getParameter("play");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("play")));
            p = params.getParameter("stop");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("stop")));
            p = params.getParameter("reverse");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("reverse")));
            p = params.getParameter("trigger");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("trigger")));
            p = params.getParameter("monitor");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("monitor")));
            p = params.getParameter("mute");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("mute")));
            p = params.getParameter("solo");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("solo")));
            p = params.getParameter("inputLevel");
            p->setValueNotifyingHost(state->getProperty("inputLevel"));
            p = params.getParameter("outputLevel");
//            PreviousOutputLevel = -1;
            p->setValueNotifyingHost(state->getProperty("outputLevel"));
            p = params.getParameter("mode");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("mode")));
            p = params.getParameter("snap");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("snap")));
            p = params.getParameter("bounce");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("bounce")));
            p = params.getParameter("nextLoop");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("nextLoop")));
            p = params.getParameter("previousLoop");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("previousLoop")));
            p = params.getParameter("newLoop");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("newLoop")));
            p = params.getParameter("removeLoop");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("removeLoop")));
            p = params.getParameter("loopSelect");
            p->setValueNotifyingHost(state->getProperty("loopSelect"));
            p = params.getParameter("fixed");
            p->setValueNotifyingHost(state->getProperty("fixed"));
            
            params.addParameterListener("record", this);
            params.addParameterListener("play", this);
            params.addParameterListener("stop", this);
            params.addParameterListener("reverse", this);
            params.addParameterListener("trigger", this);
            params.addParameterListener("mute", this);
            params.addParameterListener("solo", this);
            params.addParameterListener("monitor", this);
            params.addParameterListener("undo", this);
            params.addParameterListener("redo", this);
            params.addParameterListener("reset", this);
            params.addParameterListener("inputLevel", this);
            params.addParameterListener("outputLevel", this);
            params.addParameterListener("mode", this);
            params.addParameterListener("snap", this);
            params.addParameterListener("bounce", this);
            params.addParameterListener("nextLoop", this);
            params.addParameterListener("previousLoop", this);
            params.addParameterListener("newLoop", this);
            params.addParameterListener("removeLoop", this);
            params.addParameterListener("loopSelect", this);
            params.addParameterListener("fixed", this);

        }
    }else{
        if(isActive()){
            params.removeParameterListener("record", this);
            params.removeParameterListener("play", this);
            params.removeParameterListener("stop", this);
            params.removeParameterListener("reverse", this);
            params.removeParameterListener("trigger", this);
            params.removeParameterListener("mute", this);
            params.removeParameterListener("solo", this);
            params.removeParameterListener("monitor", this);
            params.removeParameterListener("undo", this);
            params.removeParameterListener("redo", this);
            params.removeParameterListener("reset", this);
            params.removeParameterListener("inputLevel", this);
            params.removeParameterListener("outputLevel", this);
            params.removeParameterListener("mode", this);
            params.removeParameterListener("snap", this);
            params.removeParameterListener("bounce", this);
            params.removeParameterListener("nextLoop", this);
            params.removeParameterListener("previousLoop", this);
            params.removeParameterListener("newLoop", this);
            params.removeParameterListener("removeLoop", this);
            params.removeParameterListener("loopSelect", this);
            params.removeParameterListener("fixed", this);

        }
    }
    active = newValue;
}

void Track::parameterChanged(const String &parameterID, float newValue) {
    if(parameterID == "inputLevel"){
        auto p = params.getParameter("inputLevel");
        state->setProperty(parameterID, p->convertTo0to1(newValue), nullptr);
        //  DBG("input: "+String(p->convertTo0to1(newValue)));
        return;
    }
    if(parameterID == "outputLevel"){
        auto p = params.getParameter("outputLevel");
        state->setProperty(parameterID, p->convertTo0to1(newValue), nullptr);
        return;
    }
    state->setProperty(parameterID, newValue, nullptr);
    
    if(parameterID == "record"){
        processRecordingChange();
    }else if(parameterID == "play"){
        processPlayChange();
    }else if(parameterID == "stop"){
        processStopChange();
    }else if(parameterID == "reverse"){
        processReverseChange();
    }else if(parameterID == "trigger"){
        processTriggerModeChange();
    }else if(parameterID == "mute"){
        processMuteChange();
    }else if(parameterID == "solo"){
        processSoloChange();
    }else if(parameterID == "undo"){
        processPreviousChange();
    }else if(parameterID == "redo"){
        processNextChange();
    }else if(parameterID == "reset"){
        processResetChange();
    }else if (parameterID == "mode") {
        processRecModeChange();
    }else if (parameterID == "bounce") {
        processBounceChange();
    }else if(parameterID == "nextLoop"){
        processLoopChange(ActiveLoop->Index + 1);
    }else if(parameterID == "previousLoop"){
        processLoopChange(ActiveLoop->Index - 1);
    }else if(parameterID == "newLoop"){
        processNewLoop();
    }else if (parameterID == "removeLoop") {
        processRemoveLoop();
    }else if (parameterID == "loopSelect") {
        processLoopChange(int(newValue));
    }else if (parameterID == "fixed") {
        fixedSize = int(newValue);
    }
        
}

void Track::RegisterLoop(int loopIdx){
    if(loopIdx >= 0 && loopIdx < loops.size()){
        ActiveLoop = loops[loopIdx];
        CurrentTop = &(ActiveLoop->CurrentTop);
        Progress = &(ActiveLoop->Progress);
        LoopDuration = &(ActiveLoop->LoopDuration);
        CurrentPlayingIndex = &(ActiveLoop->CurrentPlayingIndex);
        Layers = ActiveLoop->Layers;
//        if(Layers->size() < 1){
//            AddLayer(true);
//        }
        *CurrentPlayingIndex = 0;
        if (InternalSynchronizer* s = dynamic_cast<InternalSynchronizer*>(primarySynchronizer.get())){
            s->setCurrentPosition(CurrentPlayingIndex);
        }
        if (InternalSynchronizer* s = dynamic_cast<InternalSynchronizer*>(secondarySynchronizer.get())){
            s->setCurrentPosition(&CurrentRecordingIndex);
        }
    }
}

void Track::AddLoop(){
    auto* l = new Loop(loops.size());
    loops.add(l);
}

void Track::RemoveLoop(){
    if(loops.size() > 1){
        loops.removeLast();
    }
}

std::shared_ptr<Layer> Track::AddLayer(bool incrementTop) {
    LayersReady = false;
    ActiveLoop->AddLayer(incrementTop, context);
    LayersReady = true;
    return Layers->back();
}



void Track::RemoveTopLayer() {
    Layers->pop_back();
    if (*CurrentTop > int(Layers->size()) - 1)
        *CurrentTop = int(Layers->size()) -1;
}

void Track::RemoveAllLayers() {
    while (Layers->size() > 0) {
        Layers->pop_back();
    }
    Layers->clear();
    *CurrentTop = -1;
    setActivePlaybackLayer(nullptr);
    setActiveRecordingLayer(nullptr);
}

int Track::BounceHistory(int startCheckPoint, int endCheckPoint) {
    int startIdx = -1;
    std::shared_ptr<AudioBuffer<float> > startBuffer = nullptr;
    for (uint i = 0; i < Layers->size(); i++) {
        if ((*Layers)[i]->Checkpoint == startCheckPoint) {
            startIdx = i;
            startBuffer = (*Layers)[i]->Buffer;
        }
        if (-1 < startIdx && startIdx < int(i)) {
            for (uint j = 0; j < context->audioInputsCount; j++) {
                if (startBuffer) {
                    startBuffer->copyFrom(j, 0, *((*Layers)[i]->Buffer), j, 0, context->allocatedLength);
                }
            }
        }
        if ((*Layers)[i]->Checkpoint == endCheckPoint)
            break;
    }
    auto l = (*Layers)[*CurrentTop];
    setActivePlaybackLayer(l);
    setActiveRecordingLayer(l);

    return startIdx;
}

void Track::BounceAllHistory() {
    int limit = int(Layers->size()) - 1;
    if(Layers->size() < 1){
        return;
    }
    if (uint((*Layers)[Layers->size() - 1]->Buffer->getNumChannels()) < context->audioInputsCount) {
        --limit;
    }
    if (limit < 1) {
        return;
    }
    for (uint i = 1; i < Layers->size(); i++) {
        for (uint j = 0; j <context->audioInputsCount; j++) {
            (*Layers)[0]->Buffer->addFrom(j, 0, (*Layers)[i]->Buffer->getReadPointer(j,0), *LoopDuration, 1);
        }
    }
    
    while (Layers->size() > 1) {
        RemoveTopLayer();
    }
    auto l = (*Layers)[0];
    setActivePlaybackLayer(l);
    setActiveRecordingLayer(l);
    UpdateLoopVisualizer();
}

int Track::getAdjustedLoopPosition(int currentIndex, int adjustment){
    if (Reverse) {
        adjustment *= -1;
    }
    if (adjustment >= 0) {
        if (currentIndex + adjustment > *LoopDuration) {
            return currentIndex + adjustment - *LoopDuration;
        }
    }else{
        if (currentIndex + adjustment < 0) {
            return *LoopDuration + currentIndex + adjustment;
        }
    }
    return currentIndex + adjustment;
}


void Track::StartResetBefore(){
    if (IsPlaying()) {
        setPlayArmed(false);
        Playing = false;
    }
    RunAfters.push_back(&Track::StartResetAfter);
}

void Track::StartResetAfter(){
    *LoopDuration = 0;
    CurrentRecordingIndex = 0;
    *CurrentPlayingIndex = 0;
    RemoveAllLayers();
    Recording = false;
    Muted = false;
    *Progress = 0;
    if (guiAlive && isActive()) {
        (context->observer->*(context->observer->updatePlayPosition)) (0, Reverse);
    }
    realignment->setRealigned(true);
}

void Track::StartRecordingBefore()
{
    if (loopToBeExtended)return;
    if (getRecordMode() == kRecOverWrite)
    {
        // truncate loop duration to current position (+ crossfade time)
        int newLoopDuration = *CurrentPlayingIndex + int(context->fadeTime + 1);
        if (newLoopDuration < *LoopDuration)
        {
            *LoopDuration = newLoopDuration;
        }
    }
    // Add a new buffer if active track doesn't have one yet
    if(getRecordMode() == 1 && Layers->size() == 0){
        AddLayer(false);
        *LoopDuration = int(context->beatsToSamples(fixedSize));
        setActiveRecordingLayer(Layers->at(0));
    }
    if ((Layers->size() < uint(1)
         || (getRecordMode() < 4
             && *CurrentTop == Layers->size() - uint(1)
             && (*Layers)[*CurrentTop]->dirty))) {
//        AddLayer(true);
//        ActiveLoop->activePlaybackLayer = (*Layers)[*CurrentTop];
//        ActiveLoop->activeRecordingLayer = (*Layers)[*CurrentTop];
    }
    else if (getRecordMode() < 4 && *CurrentTop < Layers->size() - uint(1)) {
        auto l = (*Layers)[*CurrentTop];
        setActivePlaybackLayer(l);
        setActiveRecordingLayer(l);
    }
    // actually start Recording
    Recording = true;
    CurrentRecordingIndex = *CurrentPlayingIndex;
    FirstRecordingBuffer = true;
    RunAfters.push_back(&Track::StartRecordingAfter);
}



void Track::StartRecordingAfter(){
    playStateChanged();
}

void Track::StopRecordingBefore()
{
    Recording = false;
    if (getRecordMode() != kRecPunch) {
        setRecordingArmed(false);
    }
    LastRecordingBuffer = true;
    RunAfters.push_back(&Track::StopRecordingAfter);
}

void Track::StopRecordingAfter()
{
    
    const int recordedCount = CurrentRecordingIndex;
    
    // recorded beyond previous loop -> update duration, start playback at 0
    if (recordedCount > *LoopDuration)
    {
        // in repeat mode hardcopy the content of the previous layers to the extended region
        if (getRecordMode() == kRecExtend  && *LoopDuration > 0) {
            const int tail = recordedCount % *LoopDuration;
            for (auto segments = recordedCount / *LoopDuration, h = 1; segments>0; --segments, ++h) {
                for (int i = 0; i < *CurrentTop; i++) {
                    for(uint j = 0; j < context->audioInputsCount;j++){
                        (*Layers)[i]->Buffer->copyFrom(j, h * *LoopDuration, (*Layers)[i]->Buffer->getReadPointer(j), (segments>0)?*LoopDuration:tail, 1);
                    }
                    if(context->xchange->writeGainModifierQueue->read_available()
                       && context->xchange->readGainModifierQueue->write_available()){
                        auto gm = context->xchange->writeGainModifierQueue->front();
                        context->xchange->writeGainModifierQueue->pop();
                        gm->startLevel = context->feedback;
                        gm->operation = GainModifier::OperationType::All;
                        gm->buffer = (*Layers)[i]->Buffer;
                        context->xchange->readGainModifierQueue->push(gm);
                    }
                }
            }
        }
        *LoopDuration = recordedCount;
        *CurrentPlayingIndex = 0;
//        if (InternalSynchronizer* s = dynamic_cast<InternalSynchronizer*>(primarySynchronizer.get())){
//            s->setCurrentPosition(CurrentPlayingIndex);
//        }
    }
    
    if (getRecordMode() != kRecPunch) {
        setRecordingArmed(false);
    }
    if(*CurrentTop < Layers->size() - uint(1)){
//        ActiveLoop->activePlaybackLayer = (*Layers)[*CurrentTop];
//        ActiveLoop->activeRecordingLayer = (*Layers)[*CurrentTop];
        ++(*CurrentTop);
        RemoveTopLayer();
        
    }
   // }
    if(isActive()){
        UpdateLoopVisualizer();
    }
    setStopArmed(false);
    LastRecordingBuffer = false;
    Triggered = false;
    playStateChanged();
}

void Track::StartPlaybackBefore()
{
    setPlayArmed(true);
    if(WasPlaying)WasPlaying=false;
    Playing = true;
    FirstPlaybackBuffer = true;
    RunAfters.push_back(&Track::StartPlaybackAfter);
    context->skipAlign = true;
}

void Track::StartPlaybackAfter()
{
    if(isStopArmed())setStopArmed(false);
    FirstPlaybackBuffer = false;
    int currentPlayBuffer = *CurrentTop;
    if (Recording) {
        size_t tmp = 1;
        size_t tmp2 = Layers->size();
        if (getRecordMode() == 0 &&  tmp2 > tmp) {
            currentPlayBuffer--;
        }
    }
    if(isActive() && currentPlayBuffer>=0){
        if(!(getRecordMode() == 1
           && Layers->size() == 1
             && !Layers->at(0)->dirty)){
        UpdateLoopVisualizer();
        }
    }
    playStateChanged();
    realignment->setRealigned(true);
}

void Track::StopPlaybackBefore()
{
    Playing = false;
    setPlayArmed(false);
    LastPlaybackBuffer = true;
    RunAfters.push_back(&Track::StopPlaybackAfter);
}

void Track::PausePlaybackBefore()
{
    if(!Playing)return;
    
    Playing = false;
    setPlayArmed(false);
    LastPlaybackBuffer = true;
    RunAfters.push_back(&Track::PausePlaybackAfter);
}

void Track::StopPlaybackAfter()
{
    *CurrentPlayingIndex = 0;
    LastPlaybackBuffer = false;
    setStopArmed(false);
    
    if (guiAlive && isActive()) {
        (context->observer->*(context->observer->updatePlayPosition)) (0, Reverse);
        playStateChanged();
    }
    *Progress = 0;
}

void Track::PausePlaybackAfter()
{
    Playing = false;
    LastPlaybackBuffer = false;
    WasPlaying = true;
    playStateChanged();
    realignment->setRealigned(true);
}

void Track::StartReverse()
{
//    if (Reverse == false && *LoopDuration > 0)
//    {
//        *CurrentPlayingIndex = (*LoopDuration - 1 - *CurrentPlayingIndex);
//    }
    Reverse = true;
    setReverseArmed(true);
    realignment->setRealigned(true);
}

void Track::StopReverse()
{
//    if (Reverse == true && *LoopDuration > 0)
//    {
//        *CurrentPlayingIndex = (*LoopDuration - 1 - *CurrentPlayingIndex);
//    }
    Reverse = false;
    setReverseArmed(false);
    realignment->setRealigned(true);
}

void Track::StartMuteBefore(){
    FirstMuteBuffer = true;
    RunAfters.push_back(&Track::StartMuteAfter);
    setMuteArmed(true);
    
}

void Track::StartMuteAfter(){
    FirstMuteBuffer = false;
    Muted = true;
    playStateChanged();
    realignment->setRealigned(true);
}

void Track::StartSoloBefore(){
    FirstSoloBuffer = Playing && !Muted;
    RunAfters.push_back(&Track::StartSoloAfter);
    setSoloArmed(true);
    context->skipAlign = true;

    //context->logMessage("start solo track " + String(Index));
}

void Track::StartSoloAfter(){
    FirstSoloBuffer = false;
    Soloed = true;
    // context->logMessage("stop solo track " + String(Index));
    playStateChanged();
}

void Track::StopMuteBefore(){
    LastMuteBuffer = true;
    Muted = false;
    RunAfters.push_back(&Track::StopMuteAfter);
    setMuteArmed(false);
    context->skipAlign = true;
}


void Track::StopMuteAfter(){
    LastMuteBuffer = false;
    playStateChanged();
}

void Track::StopSoloBefore(){
    LastSoloBuffer = Playing && !Muted;
    Soloed = false;
    RunAfters.push_back(&Track::StopSoloAfter);
    setSoloArmed(false);
}


void Track::StopSoloAfter(){
    LastSoloBuffer = false;
    //context->logMessage("stop solo track " + String(Index));
    playStateChanged();
}

void Track::ChangeLoopBefore(int newLoopIdx){
    if(newLoopIdx >= -1 && newLoopIdx <= loops.size()){
        if(newLoopIdx == loops.size()){
            newLoopIdx = 0;
        }
        if(newLoopIdx == -1){
            newLoopIdx = loops.size() - 1;
        }
        nextLoop = newLoopIdx;
        RunAfters.push_back(&Track::ChangeLoopAfter);
        if(Recording){
            StopRecordingBefore();
            WasRecording = true;
        }
    }
    LastPlaybackBuffer = true;
    loopChangeArmed = false;
}


void Track::ChangeLoopAfter(){
    RegisterLoop(nextLoop);
    LastPlaybackBuffer = false;
    FirstPlaybackBuffer = true;
    if(WasRecording){
        setRecordingArmed(true);
        StartRecordingBefore();
        std::swap(RunAfters[RunAfters.size()-1],RunAfters[RunAfters.size()-2]);
        WasRecording = false;
    }
    if(guiAlive){
        (context->observer->*(context->observer->updatePlayPosition)) (0, Reverse);
        (context->observer->*(context->observer->loopChange)) (this->Index, nextLoop);
    }
    UpdateLoopVisualizer();
    realignment->setRealigned(true);
    context->skipAlign = true;
    refresh = true;
}


void Track::UpdateLoopVisualizer(Layer* l){
    if (guiAlive) {
        if (context->xchange->writeVisualisationBufferQueue->read_available()
            && context->xchange->readVisualisationBufferQueue->write_available()) {
            std::shared_ptr<BufferForVisualisation> b;
            context->xchange->writeVisualisationBufferQueue->pop(b);
            if(nullptr != l){
                b->buffer = l->Buffer;
                b->numSamples = *LoopDuration;
                b->layerIndex = l->index;
            }else{
                b->buffer = nullptr;
                b->numSamples = 0;
            }
            (context->observer->*(context->observer->layerChange)) ( Index,b->layerIndex);
            context->xchange->readVisualisationBufferQueue->push(b);
        }
    }
}

void Track::UpdateLoopVisualizer(){

    if (*CurrentTop >= 0) {
        int index = std::max((*Layers)[*CurrentTop]->dirty ? *CurrentTop : *CurrentTop - 1, 0);
        UpdateLoopVisualizer((*Layers)[index].get());
    }else{
        UpdateLoopVisualizer(nullptr);
    }
}

void Track::RefreshLoopVisualizer(){
   // int i = std::max(-1,int(Layers->size())-LAYERS_VISIBLE)-1;
    context->xchange->readVisualisationBufferQueue->reset();
    int i = -1;
    while (++i < Layers->size()
           && (*Layers)[i]->dirty) {
        UpdateLoopVisualizer((*Layers)[i].get());
    }
	refresh = false;
	(context->observer->*(context->observer->layersRefreshed))();
}

void Track::RemoveLoopBefore(){
    if(loops.size() > 1){
        if (ActiveLoop->Index == uint(loops.size() -1)){
            ChangeLoopBefore(ActiveLoop->Index -1);
            RunAfters.push_back(&Track::RemoveLoopAfter);
        }else{
            RemoveLoopAfter();
        }
    }
}


void Track::RemoveLoopAfter(){
    RemoveLoop();
    if(guiAlive){
        (context->observer->*(context->observer->loopRemoval)) ();
    }
    realignment->setRealigned(true);
}

void Track::SetPreviousBefore(){
    auto l = getActivePlaybackLayer();
    if(nullptr != l
       && nullptr != l->Buffer
        && l->index > 0){
            RunAfters.push_back(&Track::SetPreviousAfter);
            getActivePlaybackLayer()->LastLayerBuffer = true;
    }
}

void Track::SetPreviousAfter(){
    getActivePlaybackLayer()->LastLayerBuffer = false;
    if (*CurrentTop > 0) {
        if(context->maxUndoHistory > -1 && context->maxUndoHistory < Layers->size()){
            if (*CurrentTop < Layers->size() - uint(context->maxUndoHistory)) {
                RemoveTopLayer();
            }
        }
        --(*CurrentTop);
    }
    setActivePlaybackLayer((*Layers)[*CurrentTop]);
    (context->observer->*(context->observer->layerChange)) (this->Index, getActivePlaybackLayer()->index);
}

void Track::SetNextBefore(){
    RunAfters.push_back(&Track::SetNextAfter);
}

int Track::getLimit(){
    int limit = int(Layers->size()) - 1;
    if (limit > 0) {
        if(!(*Layers)[limit]->dirty){
            --limit;
        }
    }
    return limit;
}

void Track::SetNextAfter(){
    int limit = getLimit();
    if (limit <=0) return;
    if (*CurrentTop < limit) {
        ++(*CurrentTop);
    }
    setActivePlaybackLayer((*Layers)[*CurrentTop]);
    getActivePlaybackLayer()->FirstLayerBuffer = true;
    (context->observer->*(context->observer->layerChange)) (this->Index, getActivePlaybackLayer()->index);
}
//-------------------process functions---------------------

void Track::processRecordingChange() {
    bool wasRecording = Recording;
    if (!wasRecording && isRecordingArmed()) {
        if (Muted) {
            setMuteArmed(false);
            Muted = false;
        }
        // start recording, if not in auto trigger mode nor snap§
        if ((!isAutoTrigger()) && (getSnapMode() == kSnapNone)) {
            StartRecordingBefore();
        }
    }
    // stop Recording, if not in snapping mode
    if (wasRecording && !isRecordingArmed() && (getSnapMode() == kSnapNone))
    {
        StopRecordingBefore();
    }
   // if (!isRecordingArmed()) {
        if(!IsPlaying() && !isStopArmed()){
            setPlayArmed(true);
            processPlayChange();
        }
  //  }
    playStateChanged();
}

void Track::processPlayChange(){
    playStateChanged();
}

void Track::playStateChanged() {
    if (guiAlive && isActive()) {
        (context->observer->*(context->observer->playChanged)) (this->Index);
    }
}

void Track::processStopChange() {
    if (Playing) {
        setPlayArmed(false);
        processPlayChange();
    }
    else {
        *CurrentPlayingIndex = 0;
    }
    if (Recording) {
        setRecordingArmed(false);
        processRecordingChange();
    }
}

void Track::processResetChange() {
    // erase content and restart Recording to 0
    StartResetBefore();
}

void Track::processRecModeChange() {
    if (getRecordMode() > 1 && getRecordMode() < 5){
        loopToBeExtended = true;
    }else{
        loopToBeExtended = false;
    }
}

void Track::processPreviousChange() {
    SetPreviousBefore();
}

void Track::processNextChange() {
    SetNextBefore();
}

void Track::processReverseChange() {
    if (getSnapMode() == kSnapNone) // toggle right now if no sync
    {
        if (isReverseArmed())
            StartReverse();
        else
            StopReverse();
    }
}

void Track::processTriggerModeChange() {
    if (isRecordingArmed())
        Triggered = false;
}


void Track::processMuteChange() {
    setRecordingArmed(isRecordingArmed() && !isMuteArmed());
    playStateChanged();
}

void Track::processSoloChange() {
    playStateChanged();
}

void Track::processBounceChange() {
    if (Recording || Playing ) {
        return;
    }
    doBounce = true;
}

void Track::processLoopChange(int newLoopIdx){
    nextLoop = newLoopIdx;
    loopChangeArmed = true;
}

void Track::processNewLoop(){
    AddLoop();
    if(guiAlive){
        (context->observer->*(context->observer->newLoop)) ();
    }
    UpdateLoopVisualizer();
}
void Track::processRemoveLoop(){
    RemoveLoopBefore();
}


//-------------------set/get-------------------------------

void Track::setMuteArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("mute");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("mute", newValue, nullptr);
    
}
void Track::setSoloArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("solo");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("solo", newValue, nullptr);
    
}
void Track::setStopArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("stop");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("stop", newValue, nullptr);
}
void Track::setInputLevel(float newValue){
    if(isActive()){
        auto p = params.getParameter("inputLevel");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(newValue);
        }
    }
    state->setProperty("inputLevel", newValue, nullptr);
}
void Track::setOutputLevel(float newValue){
    if(isActive()){
        auto p = params.getParameter("outputLevel");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(newValue);
        }
    }
    state->setProperty("outputLevel", newValue, nullptr);
}
void Track::setReverseArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("reverse");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("reverse", newValue, nullptr);
}
void Track::setPlayArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("play");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("play", newValue, nullptr);
}
void Track::setMonitoring(bool newValue){
    if(isActive()){
        auto p = params.getParameter("monitor");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("monitoring", newValue, nullptr);
}
void Track::setRecordingArmed(bool newValue){
    if(isActive()){
        auto* p = params.getParameter("record");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("record", newValue, nullptr);
}
void Track::setAutoTrigger(bool newValue){
    if(isActive()){
        auto p = params.getParameter("trigger");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("trigger", newValue, nullptr);
}

std::shared_ptr<Layer> Track::getActivePlaybackLayer(){
    if(nullptr == ActiveLoop->activePlaybackLayer){
        *CurrentTop = getLimit();
        if(*CurrentTop>=0){
            setActivePlaybackLayer((*Layers)[*CurrentTop]);
        }    }
//    if(!ActiveLoop->activePlaybackLayer->dirty){
//        auto idx = getLimit();
//        while( idx >0 && !ActiveLoop->Layers->at(*CurrentTop)->dirty){
//            idx=std::max(idx-1, 0);
//        }
//        setActivePlaybackLayer((*Layers)[idx]);
//    }
    return ActiveLoop->activePlaybackLayer;
}
void Track::setActivePlaybackLayer(std::shared_ptr<Layer> l){
    ActiveLoop->activePlaybackLayer = l;
}
std::shared_ptr<Layer> Track::getActiveRecordingLayer(){
    return ActiveLoop->activeRecordingLayer;
    
}
void Track::setActiveRecordingLayer(std::shared_ptr<Layer> l){
    ActiveLoop->activeRecordingLayer = l;
}



bool Track::isMuteArmed() {  return state->getProperty("mute"); }
bool Track::isSoloArmed() {  return state->getProperty("solo"); }
bool Track::isStopArmed() {  return state->getProperty("stop"); }
float Track::getInputLevel() {  return state->getProperty("inputLevel"); }
float Track::getOutputLevel() {  return state->getProperty("outputLevel"); }
bool Track::isReverseArmed() {  return state->getProperty("reverse"); }
bool Track::isPlayArmed() {  return state->getProperty("play"); }
bool Track::isMonitoring() {  return state->getProperty("monitor"); }
bool Track::isRecordingArmed() {  return state->getProperty("record"); }
bool Track::isAutoTrigger() {  return state->getProperty("trigger"); }
SnapMode Track::getSnapMode() {  return SnapMode( int(state->getProperty("snap"))); }
RecordMode Track::getRecordMode() {  return RecordMode( int(state->getProperty("mode"))); }

