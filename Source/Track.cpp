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
                                                                                                        , params(p)
                                                                                                        , context(c){
	Index = index;
	RunAfters.reserve(10);
    state = new ValueTree(
                          { "TrackParams", {{ "index", int(index) }},
                {
                    { "Parameter", {{ "id", "record" }, { "value", false }}},
                    { "Parameter", {{ "id", "play" },     { "value", false }}},
                    { "Parameter", {{ "id", "stop" },     { "value", false }}},
                    { "Parameter", {{ "id", "reverse" },     { "value", false }}},
                    { "Parameter", {{ "id", "trigger" },     { "value", false }}},
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
                    { "Parameter", {{ "id", "loopSelect" }, { "value" , 0 }}}

                }
            });

    state->setProperty("inputLevel", 0.7f, nullptr);
    state->setProperty("outputLevel", 0.5f, nullptr);
    state->setProperty("record", false, nullptr);
    state->setProperty("play", false, nullptr);
    state->setProperty("stop", false, nullptr);
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
}
 
 
Track::~Track() {
  
    RemoveAllLayers();
    setActive(false);
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

		}
    }
    active = newValue;
}

void Track::parameterChanged(const String &parameterID, float newValue) {
    if(parameterID == "inputLevel"){
        PreviousInputLevel = state->getProperty(parameterID);
        state->setProperty(parameterID, Decibels::decibelsToGain(newValue), nullptr);
      //  DBG("input: "+String(p->convertTo0to1(newValue)));
        return;
    }
    if(parameterID == "outputLevel"){
        PreviousOutputLevel = state->getProperty(parameterID);
        state->setProperty(parameterID, Decibels::decibelsToGain(newValue), nullptr);
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
        processLoopChange(newValue);
    }

}

void Track::RegisterLoop(int loopIdx){
    if(loopIdx >= 0 && loopIdx < loops.size()){
        ActiveLoop = loops[loopIdx];
        CurrentTop = &(ActiveLoop->CurrentTop);
        Progress = &(ActiveLoop->Progress);
        LoopDuration = &(ActiveLoop->LoopDuration);
        CurrentPlayingIndex = &(ActiveLoop->CurrentPlayingIndex);
        Layers = &(ActiveLoop->Layers);
        if(Layers->size() < 1){
            AddLayer(true);
        }
        *CurrentPlayingIndex = 0;
    }
}

void Track::AddLoop(){
    auto* l = new Loop();
    l->Index = loops.size();
    loops.add(l);
    l->Layers.reserve(100);
}

void Track::RemoveLoop(){
    if(loops.size() > 1){
        loops.removeLast();
    }
}

void Track::AddLayer(bool incrementTop) {
	LayersReady = false;
	ActiveLoop->AddLayer(incrementTop, context);
	LayersReady = true;
}



void Track::RemoveTopLayer() {

	auto* layer = Layers->back();
	delete layer->Buffer;
	Layers->pop_back();

	if (*CurrentTop > int(Layers->size()) - 1)
		*CurrentTop = int(Layers->size()) -1;
}

void Track::RemoveAllLayers() {
	while (Layers->size() > 0) {
		auto* layer = Layers->back();
		delete layer->Buffer;
		Layers->pop_back();
	}
    Layers->clear();
    *CurrentTop = -1;
}

int Track::BounceHistory(int startCheckPoint, int endCheckPoint) {
	int startIdx = -1;
	AudioBuffer<float>* startBuffer = nullptr;
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
	return startIdx;
}

void Track::BounceAllHistory() {
	int limit = int(Layers->size()) - 1;
	if(Layers->size() < 1){
		return;
	}
	if ((*Layers)[Layers->size() - 1]->Buffer->getNumChannels() < context->audioInputsCount) {
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
    UpdateLoopVisualizer();
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

	if ((Layers->size() < uint(1)
         || (getRecordMode() < 3
            && *CurrentTop == Layers->size() - uint(1)
            && (*Layers)[*CurrentTop]->Dirty))) {
		AddLayer(true);
	}
	else if (getRecordMode() < 3 && *CurrentTop < Layers->size() - uint(1)) {
		++(*CurrentTop);
	}
	// actually start Recording
	Recording = true;
	CurrentRecordingIndex = *CurrentPlayingIndex;
    FirstRecordingBuffer = true;
    RunAfters.push_back(&Track::StartRecordingAfter);
}



void Track::StartRecordingAfter(){
    FirstRecordingBuffer = false;
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
                    for(int j = 0; j < context->audioInputsCount;j++){
                        (*Layers)[i]->Buffer->copyFrom(j, h * *LoopDuration, (*Layers)[i]->Buffer->getReadPointer(j), (segments>0)?*LoopDuration:tail, 1);
                    }
                    (*Layers)[i]->Buffer->applyGain(context->feedback);
                }
            }
		}
		*LoopDuration = recordedCount;
		*CurrentPlayingIndex = 0;
	}

	if (getRecordMode() != kRecPunch) {
        setRecordingArmed(false);
	}
    if(*CurrentTop < Layers->size() - uint(1)){
        ++(*CurrentTop);
        RemoveTopLayer();
    }
	//}
    if(isActive()){
        UpdateLoopVisualizer();
    }
    setStopArmed(false);
    LastRecordingBuffer = false;
    Triggered = false;
}

void Track::StartPlaybackBefore()
{
    setPlayArmed(true);
    if(WasPlaying)WasPlaying=false;
    Playing = true;
    FirstPlaybackBuffer = true;
    RunAfters.push_back(&Track::StartPlaybackAfter);
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
        UpdateLoopVisualizer();
    }
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
    }
    *Progress = 0;
}

void Track::PausePlaybackAfter()
{
	Playing = false;
    LastPlaybackBuffer = false;
    WasPlaying = true;
}

void Track::StartReverse()
{
	if (Reverse == false && *LoopDuration > 0)
	{
		*CurrentPlayingIndex = (*LoopDuration - 1 - *CurrentPlayingIndex);
	}
	Reverse = true;
    setReverseArmed(true);

}

void Track::StopReverse()
{
	if (Reverse == true && *LoopDuration > 0)
	{
		*CurrentPlayingIndex = (*LoopDuration - 1 - *CurrentPlayingIndex);
	}
	Reverse = false;
    setReverseArmed(false);
    
}

void Track::StartMuteBefore(){
    FirstMuteBuffer = true;
    RunAfters.push_back(&Track::StartMuteAfter);
    setMuteArmed(true);

}

void Track::StartMuteAfter(){
    FirstMuteBuffer = false;
    Muted = true;
}

void Track::StartSoloBefore(){
    FirstSoloBuffer = Playing && !Muted;
    RunAfters.push_back(&Track::StartSoloAfter);
    setSoloArmed(true);
    logger->logMessage("start solo track " + String(Index));

}

void Track::StartSoloAfter(){
    FirstSoloBuffer = false;
    Soloed = true;
    logger->logMessage("stop solo track " + String(Index));
}

void Track::StopMuteBefore(){
    LastMuteBuffer = true;
    Muted = false;
    RunAfters.push_back(&Track::StopMuteAfter);
    setMuteArmed(false);

}


void Track::StopMuteAfter(){
    LastMuteBuffer = false;
}

void Track::StopSoloBefore(){
    LastSoloBuffer = Playing && !Muted;
    Soloed = false;
    RunAfters.push_back(&Track::StopSoloAfter);
    setSoloArmed(false);

}


void Track::StopSoloAfter(){
    LastSoloBuffer = false;
    logger->logMessage("stop solo track " + String(Index));
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
    loopChangeArmed = false;
}


void Track::ChangeLoopAfter(){
    RegisterLoop(nextLoop);
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
}

void Track::UpdateLoopVisualizer(){
    if (context->xchange->writeBufferQueue->read_available() 
		&& context->xchange->readBufferQueue->write_available() 
		&& *CurrentTop >= 0) {
        BufferForVisualisation* b;
        context->xchange->writeBufferQueue->pop(b);
        for (int i = 0; i < context->audioInputsCount; ++i) {
			int index = std::max((*Layers)[*CurrentTop]->Dirty ? *CurrentTop : *CurrentTop - 1, 0);

            b->buffer->copyFrom(i, 0, *(*Layers)[index]->Buffer, i, 0, *LoopDuration);
        }
        b->numSamples = *LoopDuration;
        context->xchange->readBufferQueue->push(b);
    }
}


void Track::RemoveLoopBefore(){
    if(loops.size() > 1){
        if (ActiveLoop->Index == loops.size() -1){
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
    if (wasRecording && !isRecordingArmed()) {
        if(!IsPlaying() && !isStopArmed()){
            setPlayArmed(true);
            processPlayChange();
        }
    }
}

void Track::processPlayChange(){

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
	loopToBeExtended = true;
}

void Track::processPreviousChange() {
    if (*CurrentTop > 0) {
        if(context->maxUndoHistory > -1 && context->maxUndoHistory < Layers->size()){
            if (*CurrentTop < Layers->size() - uint(context->maxUndoHistory)) {
                RemoveTopLayer();
            }
        }
            --(*CurrentTop);
    }
    UpdateLoopVisualizer();
}

void Track::processNextChange() {
    int limit = int(Layers->size()) - 1;
    if (limit > 0) {
        if(!(*Layers)[limit]->Dirty){
            --limit;
        }
    }else{
        return;
    }
    if (*CurrentTop < limit) {
        ++(*CurrentTop);
    }
    UpdateLoopVisualizer();
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
}

void Track::processSoloChange() {
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
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("mute", newValue, nullptr);
    
}
void Track::setSoloArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("solo");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("solo", newValue, nullptr);
    
}
void Track::setStopArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("stop");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
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
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("reverse", newValue, nullptr);
}
void Track::setPlayArmed(bool newValue){
    if(isActive()){
        auto p = params.getParameter("play");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("play", newValue, nullptr);
}
void Track::setMonitoring(bool newValue){
    if(isActive()){
        auto p = params.getParameter("monitor");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("monitoring", newValue, nullptr);
}
void Track::setRecordingArmed(bool newValue){
    if(isActive()){
        auto* p = params.getParameter("record");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("record", newValue, nullptr);
}
void Track::setAutoTrigger(bool newValue){
    if(isActive()){
        auto p = params.getParameter("trigger");
        if(p->getValue() != newValue){
            p->setValueNotifyingHost(p->convertTo0to1(newValue));
        }
    }
    state->setProperty("trigger", newValue, nullptr);
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

