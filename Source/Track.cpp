//
//  Track.cpp
//  Orbish
//
//  Created by Duke Quarcoo on 23/03/2019.
//  Copyright © 2019 EXU. All rights reserved.
//
#pragma once

#include "Track.h"
#include "Realignment.h"

#include <math.h>
#include <mutex>



Track::Track(uint index, bool a, AudioProcessorValueTreeState& p, std::shared_ptr<OrbishContext> &c, bool& gui) : guiAlive(gui)
, refresh(false)
, params(p)
, context(c){
    Index = index;
    RunAfters.reserve(10);
    CurrentPlayingIndex = &CurrentRecordingIndex;
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
            { "Parameter", {{ "id", "nextRecMode" },     { "value", false }}},
            { "Parameter", {{ "id", "snap" },     { "value", 1 }}},
            { "Parameter", {{ "id", "nextSnapMode" },     { "value", false }}},
            { "Parameter", {{ "id", "incFixed" },     { "value", false }}},

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
    state->setProperty("nextRecMode", false, nullptr);
    state->setProperty("snap", 1, nullptr);
    state->setProperty("nextSnapMode", false, nullptr);
    state->setProperty("incFixed", false, nullptr);
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
    snap = state->getProperty("snap");
    recMode = state->getProperty("mode");
    input = state->getProperty("inputLevel");
    output = state->getProperty("outputLevel");

    AddLoop();
    RegisterLoop(loops.size()-1);
    realignment = new Realignment(context->samplesPerBlock, context->sampleRate);
}

void Track::setState(ValueTree* inputState){
    
}

Track::~Track() {
    
    RemoveAllLayers();
    setActive(false);
    delete realignment;
    delete state;
    ActiveLoop = nullptr;
    loops.clear();
}

int Track::getNextSynchronizationPoint(SnapMode snapMode){
    if(Recording){
        return secondarySynchronizer->getNextSynchronizationPoint(snapMode);
    }else if(Playing && *LoopDuration > 0){
        return primarySynchronizer->getNextSynchronizationPoint(snapMode);
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
            p = params.getParameter("nextRecMode");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("nextRecMode")));
            p = params.getParameter("snap");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("snap")));
            p = params.getParameter("nextSnapMode");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("nextSnapMode")));
            p = params.getParameter("incFixed");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("incFixed")));
            p = params.getParameter("bounce");
            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("bounce")));
//            p = params.getParameter("nextLoop");
//            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("nextLoop")));
//            p = params.getParameter("previousLoop");
//            p->setValueNotifyingHost(p->convertTo0to1(state->getProperty("previousLoop")));
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
            params.addParameterListener("nextRecMode", this);
            params.addParameterListener("snap", this);
            params.addParameterListener("nextSnapMode", this);
            params.addParameterListener("incFixed", this);
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
            params.removeParameterListener("nextRecMode", this);
            params.removeParameterListener("snap", this);
            params.removeParameterListener("nextSnapMode", this);
            params.removeParameterListener("incFixed", this);
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
    try {
    state->setProperty(parameterID, newValue, nullptr);
    if(parameterID == "inputLevel"){
        auto p = params.getParameter("inputLevel");
        state->setProperty(parameterID, p->convertTo0to1(newValue), nullptr);
        //  DBG("input: "+String(p->convertTo0to1(newValue)));
        input = Decibels::decibelsToGain(newValue);
;
        return;
    }
    if(parameterID == "outputLevel"){
        auto p = params.getParameter("outputLevel");
        state->setProperty(parameterID, p->convertTo0to1(newValue), nullptr);
        output = Decibels::decibelsToGain(newValue);
        return;
    }
    if(parameterID == "record"){
        processRecordingChange();
        recordingArmed = state->getProperty("record");
    }else if(parameterID == "play"){
        processPlayChange();
        playArmed = state->getProperty("play");
    }else if(parameterID == "stop"){
        stopArmed = state->getProperty("stop");
        if(stopArmed){
            processStopChange();
        }
    }else if(parameterID == "monitor"){
        monitoring = state->getProperty("monitor");
    }else if(parameterID == "reverse"){
        processReverseChange();
        reverseArmed = state->getProperty("reverse");
    }else if(parameterID == "trigger"){
        processTriggerModeChange();
        trigger = state->getProperty("trigger");
    }else if(parameterID == "mute"){
        processMuteChange();
        muteArmed = state->getProperty("mute");
    }else if(parameterID == "solo"){
        processSoloChange();
        soloArmed = state->getProperty("solo");
    }else if(parameterID == "undo"){
        processPreviousChange();
    }else if(parameterID == "redo"){
        processNextChange();
    }else if(parameterID == "reset"){
        processResetChange();
    }else if (parameterID == "mode") {
        processRecModeChange();
    }else if (parameterID == "nextRecMode") {
        processNextRecModeChange();
    }else if (parameterID == "snap") {
        processSnapModeChange();
    }else if (parameterID == "nextSnapMode") {
        processNextSnapModeChange();
    }else if (parameterID == "incFixed") {
        processIncFixedChange();
    }else if (parameterID == "bounce") {
        processBounceChange();
    }else if(parameterID == "nextLoop"){
         processNextLoop();
    }else if(parameterID == "previousLoop"){
         processPreviousLoop();
    }else if(parameterID == "newLoop"){
        processNewLoop();
    }else if (parameterID == "removeLoop") {
        processRemoveLoop();
    }else if (parameterID == "loopSelect") {
        processLoopChange(int(newValue));
    }else if (parameterID == "fixed") {
        fixedSize = int(newValue);
    }
    } catch (...) {
        DBG("Exception in Track::parameterChanged for: " + parameterID);
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
            s->setLoopEnd(LoopDuration);
        }
        if (InternalSynchronizer* s = dynamic_cast<InternalSynchronizer*>(secondarySynchronizer.get())){
            s->setCurrentPosition(&CurrentRecordingIndex);
            s->setLoopEnd(LoopDuration);
        }
    }
}

void Track::AddLoop(){
    auto* l = new Loop(loops.size());
    loops.add(l);
}

void Track::RemoveLoop(){
    if(loops.size() > 1){
        auto l = loops.getLast();
        if (l == ActiveLoop){
            ActiveLoop = loops[l->Index - 1];
        }
        l->Layers = nullptr;
        loops.removeLast();
    }
}

std::shared_ptr<Layer> Track::AddLayer(bool incrementTop) {
    LayersReady = false;
    ActiveLoop->AddLayer(incrementTop, context);
    LayersReady = true;
    if (Layers->empty()) return nullptr;
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
                    (*Layers)[i].reset();
                }
            }
        }
        if ((*Layers)[i] && (*Layers)[i]->Checkpoint == endCheckPoint)
            break;
    }
    int ct = safeCurrentTop();
    if (ct >= 0) {
        auto l = (*Layers)[ct];
        setActivePlaybackLayer(l);
        setActiveRecordingLayer(l);
    }

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
    updateVisualizationBuffers();
//    UpdateLoopVisualizer();
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
//        setPlayArmed(false);
        Playing = false;
    }
    RunAfters.push_back(&Track::StartResetAfter);
}

void Track::updateVisualizationBuffers(){
    auto l = getActiveRecordingLayer();
    if (nullptr == l) {
        return;
    }
    l->makeVisualizationBuffer(*LoopDuration);
    getActivePlaybackLayer();
    ActiveLoop->updateFlattenedVisualizationBuffer();
}

void Track::StartResetAfter(){
    *LoopDuration = 0;
    CurrentRecordingIndex = 0;
    *CurrentPlayingIndex = 0;
    RemoveAllLayers();
    Recording = false;
    Muted = false;
    *Progress = 0;
    ActiveLoop->clearFlattenedVisualizationBuffer();
    if (guiAlive && isActive()) {
        (context->observer->*(context->observer->updatePlayPosition)) (0, Reverse);
    }
    realignment->setRealigned(true);
    // Reset stretcher to clear stale buffered audio
    if (timeStretcher) {
        timeStretcher->prepare(context->sampleRate, context->audioInputsCount,
                               context->samplesPerBlock);
        timeStretcher->primeWithSilence();
        stretcherStartDelay = timeStretcher->getStartDelay();
        stretcherDelayConsumed = 0;
    }
    originalTempo = 0.0;
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
        *LoopDuration = int(context->beatsToSamples(fixedSize));
    }
    if(Layers->size() == 0){
        originalTempo = context->bpm;
        auto l = AddLayer(false);
        if (l != nullptr) {
            setActiveRecordingLayer(l);
        }
        // If l is nullptr (layerQueue empty), recording layer will be
        // created in handleRecordBlock when the queue is populated.
    }else{
//         || (getRecordMode() < 4
//             && *CurrentTop == Layers->size() - uint(1)
//             && (*Layers)[*CurrentTop]->dirty))) {
        int ct = safeCurrentTop();
        if (ct >= 0) {
            auto l = (*Layers)[ct];
            setActivePlaybackLayer(l);
            setActiveRecordingLayer(l);
        } else {
            // CurrentTop is invalid, reset to last layer
            *CurrentTop = (int)Layers->size() - 1;
            if (*CurrentTop >= 0) {
                auto l = (*Layers)[*CurrentTop];
                setActivePlaybackLayer(l);
                setActiveRecordingLayer(l);
            }
        }
    }
//    ActiveLoop->activeRecordingLayer = (*Layers)[*CurrentTop];
    // actually start Recording
    Recording = true;
    CurrentRecordingIndex = std::max(*CurrentPlayingIndex, 0);
    DBG(String("CurrentRecordingIndex in start rec befor")+String(CurrentRecordingIndex));

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
                for (int i = 0; i < *CurrentTop && i < (int)Layers->size(); i++) {
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
        updateVisualizationBuffers();
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
//    if(isActive()){
//        UpdateLoopVisualizer();
//    }
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
             && !(*Layers)[0]->dirty)){
//        UpdateLoopVisualizer();
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
    nextLoop = newLoopIdx % loops.size();
    if (newLoopIdx < 0){
        nextLoop = nextLoop + loops.size();
    }
    RunAfters.push_back(&Track::ChangeLoopAfter);
    if(Recording){
        StopRecordingBefore();
        WasRecording = true;
    }
    LastPlaybackBuffer = true;
    loopChangeArmed = false;
}


void Track::ChangeLoopAfter(){
    RegisterLoop(nextLoop);
    DBG("Changing active loop:");
    DBG(nextLoop);
    LastPlaybackBuffer = false;
    FirstPlaybackBuffer = true;
    if(WasRecording){
        setRecordingArmed(true);
        StartRecordingBefore();
        if (RunAfters.size() >= 2)
            std::swap(RunAfters[RunAfters.size()-1],RunAfters[RunAfters.size()-2]);
        WasRecording = false;
    }
    if(guiAlive){
        (context->observer->*(context->observer->updatePlayPosition)) (0, Reverse);
        (context->observer->*(context->observer->loopChange)) (this->Index, nextLoop);
    } 
//    UpdateLoopVisualizer();
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
            if(guiAlive){
                (context->observer->*(context->observer->layerChange)) ( Index,b->layerIndex);
                context->xchange->readVisualisationBufferQueue->push(b);
            }
        }
    }
}

void Track::UpdateLoopVisualizer(){

    if (*CurrentTop >= 0 && *CurrentTop < (int)Layers->size()) {
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
    
    if(guiAlive){
        (context->observer->*(context->observer->layersRefreshed))();
    }
}

void Track::RemoveLoopBefore(){
    if(loops.size() > 1){
        if (ActiveLoop->Index == uint(loops.size() -1)){
            ChangeLoopBefore(ActiveLoop->Index -1);
        }
            RunAfters.push_back(&Track::RemoveLoopAfter);
    }
}


void Track::RemoveLoopAfter(){
    if(guiAlive){
        (context->observer->*(context->observer->loopRemoval)) ();
    }
    RemoveLoop();
    DBG("Removing audio loop");
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
                if(guiAlive){
                    (context->observer->*(context->observer->layerChange)) (this->Index, getActivePlaybackLayer()->index);
                }
                RemoveTopLayer();
            }
        }
        --(*CurrentTop);
    }
    int ct = safeCurrentTop();
    if (ct >= 0) {
        setActivePlaybackLayer((*Layers)[ct]);
    }
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
    int ct = safeCurrentTop();
    if (ct < 0) return;
    setActivePlaybackLayer((*Layers)[ct]);
    getActivePlaybackLayer()->FirstLayerBuffer = true;
    if(guiAlive){
        (context->observer->*(context->observer->layerChange)) (this->Index, getActivePlaybackLayer()->index);
    }
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
    recMode = state->getProperty("mode");
}

void Track::processNextRecModeChange() {
    recMode=(recMode==kLastRec)?kFirstRec:recMode+1;
    setRecMode(recMode);
}

void Track::processSnapModeChange() {
    snap = state->getProperty("snap");
}

void Track::processNextSnapModeChange() {
    snap=(snap==kLastSnap)?kFirstSnap:snap+1;
    setSnapMode(snap);
}

void Track::processIncFixedChange() {
    fixedSize=(fixedSize==16)?1:fixedSize+1;
    setFixedSize(fixedSize);
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
    context->firstRun = true;
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

void Track::processNextLoop(){
    ++nextLoop;
    loopChangeArmed = true;
}

void Track::processPreviousLoop(){
    --nextLoop;
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
    muteArmed = state->getProperty("mute");
}

void Track::setSoloArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("solo");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("solo", newValue, nullptr);
    soloArmed = state->getProperty("solo");
}
void Track::setStopArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("stop");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("stop", newValue, nullptr);
    stopArmed = state->getProperty("stop");
}

void Track::setInputLevel(float newValue){
    if(isActive()){
        auto p = params.getParameter("inputLevel");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(newValue);
        }
    }
    state->setProperty("inputLevel", newValue, nullptr);
    input = state->getProperty("inputLevel");
}

void Track::setOutputLevel(float newValue){
    if(isActive()){
        auto p = params.getParameter("outputLevel");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(newValue);
        }
    }
    state->setProperty("outputLevel", newValue, nullptr);
    output = state->getProperty("outputLevel");
}

void Track::setReverseArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("reverse");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("reverse", newValue, nullptr);
    reverseArmed = state->getProperty("reverse");
}

void Track::setPlayArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("play");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("play", newValue, nullptr);
    playArmed = state->getProperty("play");
}

void Track::setMonitoring(bool newValue){
    if(isActive()){
        auto p = params.getParameter("monitor");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    monitoring = state->getProperty("monitor");
}

void Track::setRecordingArmed(bool newValue){
    if(isActive()){
        auto* p = params.getParameter("record");
        if(p->getValue() != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("record", newValue, nullptr);
    recordingArmed = state->getProperty("record");
}

void Track::setAutoTrigger(bool newValue){
    if(isActive()){
        auto p = params.getParameter("trigger");
        if(p->convertTo0to1(p->getValue()) != float(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(float(newValue)));
        }
    }
    state->setProperty("trigger", newValue, nullptr);
    trigger = state->getProperty("trigger");
}

void Track::setRecMode(int newValue){
    if(isActive()){
        auto p = params.getParameter("mode");
        if(p->getValue() != p->convertTo0to1(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("mode", newValue, nullptr);
    recMode = state->getProperty("mode");
}

void Track::setSnapMode(int newValue){
    if(isActive()){
        auto p = params.getParameter("snap");
        if(p->getValue() != p->convertTo0to1(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("snap", newValue, nullptr);
    snap = state->getProperty("snap");
}

void Track::setFixedSize(int newValue){
    if(isActive()){
        auto p = params.getParameter("fixed");
        if(p->getValue() != p->convertTo0to1(newValue)){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("fixed", newValue, nullptr);
    fixedSize = state->getProperty("fixed");
}

std::shared_ptr<Layer> Track::getActivePlaybackLayer(){
    if (nullptr == ActiveLoop->Layers || ActiveLoop->Layers->size() == 0) {
        return nullptr;
    }
    if(nullptr == ActiveLoop->activePlaybackLayer){
        *CurrentTop = getLimit();
        int ct = safeCurrentTop();
        if(ct >= 0){
            setActivePlaybackLayer((*Layers)[ct]);
        }
    }
    
    return ActiveLoop->activePlaybackLayer;
}
void Track::setActivePlaybackLayer(std::shared_ptr<Layer> l){
    ActiveLoop->activePlaybackLayer = l;
}
std::shared_ptr<Layer> Track::getActiveRecordingLayer(){
    if (nullptr == ActiveLoop->Layers || ActiveLoop->Layers->size() == 0) {
        return nullptr;
    }
    return ActiveLoop->activeRecordingLayer;
}

void Track::setActiveRecordingLayer(std::shared_ptr<Layer> l){
    ActiveLoop->activeRecordingLayer = l;
}

bool Track::isMuteArmed() {  return muteArmed; }
bool Track::isSoloArmed() {  return soloArmed; }
bool Track::isStopArmed() {  return stopArmed; }
float Track::getInputLevel() {  return input; }
float Track::getOutputLevel() {  return output; }
bool Track::isReverseArmed() {  return reverseArmed; }
bool Track::isPlayArmed() {  return playArmed; }
bool Track::isMonitoring() {  return monitoring; }
bool Track::isRecordingArmed() {  return recordingArmed; }
bool Track::isAutoTrigger() {  return trigger; }
SnapMode Track::getSnapMode() {  return SnapMode(snap); }
RecordMode Track::getRecordMode() {  return RecordMode(recMode); }
//
//void Track::updateVisualizationBuffers(){
//    if(layerIndex < 0)return;
//    setTotalAudioLength(length);
//    readBuffer = b;
//    GLfloat * visualizationBuffer;
//    auto size = getTotalLength();
//
//
//    visualizationBuffer = new GLfloat [BUFFER_READ_SIZE];
//    FloatVectorOperations::clear(visualizationBuffer, BUFFER_READ_SIZE);
//
//    for (auto i=0; i<BUFFER_READ_SIZE; ++i) {
//        visualizationBuffer[i] =  readBuffer->getSample(0, float(i)/float(BUFFER_READ_SIZE) * size);
//    }
//    if (layerIndex >= visualizationBuffers.size()) {
//        visualizationBuffers.push_back(visualizationBuffer);
//    }else{
//        visualizationBuffers[layerIndex] = visualizationBuffer;
//    }
//    if(getDisplayType() == kFlat){
//        FloatVectorOperations::clear(flattenedVisualizationBuffer, BUFFER_READ_SIZE);
//        for(auto i=0;i<visualizationBuffers.size() && i<layerNumber;++i){
//            FloatVectorOperations::add(flattenedVisualizationBuffer, visualizationBuffers[i], BUFFER_READ_SIZE);
//        }
//    }
//}
